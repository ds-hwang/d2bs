#include <vector>

#include "D2Handlers.h"
#include "Script.h"
#include "ScreenHook.h"
#include "Unit.h"
#include "Helpers.h"
#include "Core.h"
#include "Constants.h"
#include "Events.h"
#include "Control.h"
#include "CollisionMap.h"
#include "ScriptEngine.h"
#include "Console.h"
#include "D2BS.h"

#include "debugnew/debug_new.h"

using namespace std;

Variables Vars;

DWORD WINAPI D2Thread(LPVOID lpParam)
{
	char path[_MAX_FNAME+MAX_PATH],
		 fname[_MAX_FNAME+MAX_PATH],
		 scriptPath[_MAX_FNAME+MAX_PATH],
		 debug[6],
		 blockMinimize[6],
		 quitOnHostile[6],
		 quitOnError[6],
		 maxGameTime[6],
		 startAtMenu[6],
		 disableCache[6],
		 memUsage[6];

	sprintf_s(path, sizeof(path), "%sd2bs-%d.log", Vars.szPath, GetProcessId(GetCurrentProcess()));
	sprintf_s(fname, sizeof(fname), "%sd2bs.ini", Vars.szPath);

	FILE* stream = NULL;
	freopen_s(&stream, path, "a+t", stderr);

	GetPrivateProfileString("settings", "ScriptPath", "scripts", scriptPath, _MAX_PATH, fname);
	GetPrivateProfileString("settings", "MaxGameTime", "0", maxGameTime, 6, fname);
	GetPrivateProfileString("settings", "Debug", "false", debug, 6, fname);
	GetPrivateProfileString("settings", "BlockMinimize", "false", blockMinimize, 6, fname);
	GetPrivateProfileString("settings", "QuitOnHostile", "false", quitOnHostile, 6, fname);
	GetPrivateProfileString("settings", "QuitOnError", "false", quitOnError, 6, fname);
	GetPrivateProfileString("settings", "StartAtMenu", "true", startAtMenu, 6, fname);
	GetPrivateProfileString("settings", "DisableCache", "true", disableCache, 6, fname);
	GetPrivateProfileString("settings", "MemoryLimit", "50", memUsage, 6, fname);

	sprintf_s(Vars.szScriptPath, _MAX_PATH, "%s%s", Vars.szPath, scriptPath);

	BOOL bInGame = FALSE;
	BOOL bStarterScript = FALSE;
	BOOL bClicked = FALSE;
	Vars.dwGameTime = GetTickCount();
	Vars.dwMaxGameTime = atoi(maxGameTime);
	Vars.bDebug = StringToBool(debug);
	Vars.bBlockMinimize = StringToBool(blockMinimize);
	Vars.bQuitOnHostile = StringToBool(quitOnHostile);
	Vars.bQuitOnError = StringToBool(quitOnError);
	Vars.bStartAtMenu = StringToBool(startAtMenu);
	Vars.bDisableCache = StringToBool(disableCache);
	Vars.dwMemUsage = atoi(memUsage);
	if(Vars.dwMemUsage < 1)
		Vars.dwMemUsage = 50;
	Vars.dwMemUsage *= 1024*1024;
	Vars.oldWNDPROC = NULL;
	Vars.image = NULL;
	Vars.text = NULL;

	// calculate the path to starter/default.dbj only once
	char defaultdbj[_MAX_PATH+_MAX_FNAME];
	sprintf_s(defaultdbj, sizeof(defaultdbj), "%s\\default.dbj", Vars.szScriptPath);
	char starterdbj[_MAX_PATH+_MAX_FNAME];
	sprintf_s(starterdbj, sizeof(starterdbj), "%s\\starter.dbj", Vars.szScriptPath);

	int i = 0;
	while(!Vars.bActive)
	{
		if(i >= 300)
		{
			MessageBox(0, "Failed to set hooks, exiting!", "D2BS", 0);
			return 0;
		}

		if(D2WIN_GetHwnd() && (ClientState() == ClientStateMenu || ClientState() == ClientStateInGame))
		{
			if(!Vars.oldWNDPROC)
				Vars.oldWNDPROC = (WNDPROC)SetWindowLong(D2WIN_GetHwnd(), GWL_WNDPROC, (LONG)GameEventHandler);
			if(Vars.oldWNDPROC)
			{
				DWORD mainThread = GetWindowThreadProcessId(D2WIN_GetHwnd(),0);
				if(mainThread)
				{
					if(!Vars.hKeybHook)
						Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, mainThread);
					if(!Vars.hMouseHook)
						Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, mainThread);
				}
			}
		}

		if(Vars.oldWNDPROC && (ClientState() == ClientStateMenu || ClientState() == ClientStateInGame))
		{
			char versionimg[_MAX_PATH+_MAX_FNAME];
			sprintf_s(versionimg, sizeof(versionimg), "%sversion.bmp", Vars.szPath);
			if(!Vars.image)
				Vars.image = new ImageHook(NULL, versionimg, 0, 10, 0, false, Center, Perm, false);
			if(!Vars.text)
				Vars.text = new TextHook(NULL, "D2BS " D2BS_VERSION, 0, 15, 13, 4, false, Center, Perm);
		}

		if(Vars.oldWNDPROC && Vars.hKeybHook && Vars.hMouseHook && Vars.image && Vars.text)
		{
			if(!ScriptEngine::Startup())
				return FALSE;

			Vars.bActive = TRUE;
			Log("D2BS Startup complete.");
		}

		Sleep(100);
		i++;
	}

	while(Vars.bActive)
	{
		switch(ClientState())
		{
			case ClientStateInGame:
			{
				if(GameReady())
				{
					Vars.image->SetX(D2GetScreenSizeX()/2);
					Vars.text->SetX(D2GetScreenSizeX()/2);

					if(bInGame && 
							((Vars.dwMaxGameTime > 0 && Vars.dwGameTime > 0 && 
							(GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime) ||
							(!IsTownLevel(GetPlayerArea()) &&
							(Vars.nChickenHP > 0 && Vars.nChickenHP >= GetUnitHP(D2CLIENT_GetPlayerUnit())) ||
							(Vars.nChickenMP > 0 && Vars.nChickenMP >= GetUnitMP(D2CLIENT_GetPlayerUnit())))))
						D2CLIENT_ExitGame();

					if(!bInGame)
					{
						Sleep(500);

						Vars.dwGameTime = GetTickCount();
						D2CLIENT_InitInventory();

						Print("ÿc2D2BSÿc0 :: Starting default.dbj");

						Script* script = ScriptEngine::CompileFile(defaultdbj, InGame);
						if(script)
							CreateThread(0, 0, ScriptThread, script, 0, 0);
						else
							Print("ÿc2D2BSÿc0 :: Failed to start default.dbj!");

						bInGame = TRUE;
					}
				}
				break;
			}
			case ClientStateBusy:
				break;
			case ClientStateMenu:
			{
				if(bInGame)
				{
					Vars.dwGameTime = NULL;
					bInGame = FALSE;
				}

				Vars.image->SetX(D2GetScreenSizeX()/2);
				Vars.text->SetX(D2GetScreenSizeX()/2);

				if(!bClicked)
				{
					if(Vars.bStartAtMenu)
					{
						clickControl(*p_D2WIN_FirstControl);
						bClicked = TRUE;
					}
				}

				if(!bStarterScript)
				{
					Sleep(500);

					Print("ÿc2D2BSÿc0 :: Starting starter.dbj");
					Script* script = ScriptEngine::CompileFile(starterdbj, OutOfGame);
					if(script)
						CreateThread(0, 0, ScriptThread, script, 0, 0);
					else
						Print("ÿc2D2BSÿc0 :: Failed to start starter.dbj!");
					bStarterScript = TRUE;
				}
				break;
			}
		}
		Sleep(100);
	}

	ScriptEngine::Shutdown();

	return NULL;
}

DWORD __fastcall GameInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
		return NULL;

	char* szBuffer = UnicodeToAnsi(wMsg);
	char* next_token1;
	int result = 0;

	if(szBuffer[0] == '.')
	{
		char* buf = _strdup(szBuffer);
		char* cmd = strtok_s(buf+1, " ", &next_token1);

		if(!_strcmpi(cmd, "start"))
		{
			char file[_MAX_PATH+_MAX_FNAME];
			sprintf_s(file, sizeof(file), "%s\\default.dbj", Vars.szScriptPath);
			Script* script = ScriptEngine::CompileFile(file, InGame);
			if(script)
			{
				Print("ÿc2D2BSÿc0 :: Starting default.dbj");
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			}
			else
				Print("ÿc2D2BSÿc0 :: Failed to start default.dbj!");
			result = -1;
		}
		else if(!_strcmpi(cmd, "stop"))
		{
			if(ScriptEngine::GetCount() > 0)
				Print("ÿc2D2BSÿc0 :: Stopping all scripts!");

			ScriptEngine::StopAll(true);
			result = -1;
		}
		else if(!_strcmpi(cmd, "reload"))
		{
			if(ScriptEngine::GetCount() > 0)
				Print("ÿc2D2BSÿc0 :: Stopping all scripts...");
			ScriptEngine::StopAll(true);

			if(!Vars.bDisableCache)
			{
				Print("ÿc2D2BSÿc0 :: Flushing the script cache...");
				ScriptEngine::FlushCache();
			}

			Print("ÿc2D2BSÿc0 :: Starting default.dbj...");
			char file[_MAX_PATH+_MAX_FNAME];
			sprintf_s(file, sizeof(file), "%s\\default.dbj", Vars.szScriptPath);
			Script* script = ScriptEngine::CompileFile(file, InGame);
			if(script)
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			else
				Print("ÿc2D2BSÿc0 :: Failed to start default.dbj!");
			result = -1;
		}
		else if(!_strcmpi(cmd, "flush"))
		{
			if(!Vars.bDisableCache)
			{
				Print("ÿc2D2BSÿc0 :: Flushing the script cache...");
				ScriptEngine::FlushCache();
			}
			result = -1;
		}
		else if(!_strcmpi(cmd, "exec"))
		{
			char* arg = szBuffer+6;
			if(strlen(arg) > 0)
			{
				Script* script = ScriptEngine::CompileCommand(arg);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);
			}

			result = -1;
		}
		else if(!_strcmpi(cmd, "load"))
		{
			char* arg = szBuffer+6;
			if(strlen(arg) > 0)
			{
				Print("ÿc2D2BSÿc0 :: Loading %s", arg);

				CHAR szPath[8192] = "";
				sprintf_s(szPath, sizeof(szPath), "%s\\%s", Vars.szScriptPath, arg);

				Script* script = ScriptEngine::CompileFile(szPath, InGame, true);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);
				else
					Print("ÿc2D2BSÿc0 :: Failed to load %s!", arg);
			}

			result = -1;
			delete[] buf;
		}
	}

	delete[] szBuffer;

	return result;
}

DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize)
{
	if(pPacket[0] == 0x15)
	{
		if(*(LPDWORD)&pPacket[2] == D2CLIENT_GetPlayerUnit()->dwUnitId)
			pPacket[10] = NULL;	
	}
	if(pPacket[0] == 0x95 || pPacket[0] == 0x18)
	{
		
		WORD Life = *(WORD*)&pPacket[1];
		WORD Mana = *(WORD*)&pPacket[3];

		if((Life & 0x8000) == 0x8000)
		{
			Life ^= 0x8000;
		}
		if((Mana & 0x8000) == 0x8000)
		{
			Mana ^= 0x8000;
		}
		if((Mana & 0x4000) == 0x4000)
		{
			Mana ^= 0x4000;
		}

		Mana *= 2;

		LifeEvent(Life, Mana);
	}
	else if(pPacket[0] == 0x26)
	{
		CHAR* pName = (CHAR*)pPacket+10;
		CHAR* pMessage = (CHAR*)pPacket + strlen(pName) + 11;
	
		ChatEvent(pName, pMessage);
	}
	else if(pPacket[0] == 0x5a && pPacket[1] == 0x07 && pPacket[2] == 0x08)
	{
		if(Vars.bQuitOnHostile)
		{
			D2CLIENT_ExitGame();
		}
	}
	else if(pPacket[0] == 0xA7)
	{
		if(pPacket[6] == AFFECT_JUST_PORTALED)
			return FALSE;
	}
	else if(pPacket[0] == 0x9c)	//itemDropEvent() by bobite, todo: sending all modes
	{
		if(pPacket[1] == 0x00 ||pPacket[1] == 0x02 ||pPacket[1] == 0x03 )
		{			
			char Code[5] = "";
			WORD itemX;
			WORD itemY;
			//(data+pos/8)<<(64-len-(pos&7))>>(64-len)); taken from magnet and mousepad
			//		date=packet, len= size of data being red, pos = where in the packet -1

			Code[0]=(*(unsigned __int64 *)(pPacket+141/8)<<(64-8-(141&7))>>(64-8));		
			Code[1]=(*(unsigned __int64 *)(pPacket+149/8)<<(64-8-(149&7))>>(64-8));	
			Code[2]=(*(unsigned __int64 *)(pPacket+157/8)<<(64-8-(157&7))>>(64-8));	
			Code[3]=(*(unsigned __int64 *)(pPacket+165/8)<<(64-8-(165&7))>>(64-8));	
			Code[(Code[3] == ' ' ? 3 : 4)] = '\0';
			itemX=(*(unsigned __int64 *)(pPacket+108/8)<<(64-16-(108&7))>>(64-16));	
			itemY=(*(unsigned __int64 *)(pPacket+125/8)<<(64-16-(125&7))>>(64-16));	
			itemX=itemX/2;
			//itemY=itemY/2; //only x gets /2
			WORD Mode = *(BYTE*)&pPacket[1];
			DWORD GID = *(DWORD*)&pPacket[4];

			ItemDropEvent(GID,Code,itemX,itemY,Mode);
		}
	}
	else if(pPacket[0] == 0x5a){ // SOJ and Walks Msg by bobite
		if (pPacket[1] == 0x11){ //stones
			DWORD soj = *(DWORD*)&pPacket[3];
			char mess[256]; 
			sprintf_s(mess, sizeof(mess), "%u Stones of Jordan Sold to Merchants", soj);				
			GameMsgEvent(mess);
		}
		if (pPacket[1] == 0x12){ //diablo walks
			char mess[] ="Diablo Walks the Earth";
			GameMsgEvent(mess);
		}
	}

	return TRUE;
}

LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	COPYDATASTRUCT* pCopy;
	switch(uMsg)
	{
		case WM_COPYDATA:
			pCopy = (COPYDATASTRUCT*)lParam;

			if(pCopy)
			{
				if(pCopy->dwData == 0x1337) // 0x1337 = Execute Script
				{
					Script* script = ScriptEngine::CompileCommand((char*)pCopy->lpData);
					if(script)
						CreateThread(0, 0, ScriptThread, script, 0, 0);
				}
				CopyDataEvent(pCopy->dwData, (char*)pCopy->lpData);
			}
			return TRUE;
		case WM_ACTIVATEAPP:
			if(!wParam && Vars.bBlockMinimize)
				return NULL;
			break;
		//case WM_NCDESTROY:
		//	{
		//		//Vars.bActive = FALSE;
		//		return TRUE;
		//	}
	}

	return (LONG)CallWindowProcA(Vars.oldWNDPROC, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(Vars.bBlockKeys)
		return 1;

	if(code >= 0) // removes chance of duplicate event firings - TechnoHunter
	{
		WORD repeatCount = LOWORD(lParam);
		bool altState = !!(HIWORD(lParam) & KF_ALTDOWN);
		bool previousState = !!(HIWORD(lParam) & KF_REPEAT);
		bool transitionState = !!(HIWORD(lParam) & KF_UP);
		bool isRepeat = !transitionState && repeatCount != 1;
		bool isDown = !(previousState && transitionState);
		bool isUp = previousState && transitionState;

		bool gameState = !!GameReady();
		bool chatBoxOpen = gameState ? !!D2CLIENT_GetUIState(5) : false;
		bool escMenuOpen = gameState ? !!D2CLIENT_GetUIState(9) : false;

		if(wParam == VK_HOME && !(chatBoxOpen || escMenuOpen))
		{
			if(!altState && isUp)
			{
				Console::ToggleBuffer();
			}
			return 1;
		}
		else if(wParam == VK_OEM_3 && !(chatBoxOpen || escMenuOpen))
		{
			if(altState && isUp)
				Console::TogglePrompt();
			return 1;
		}
		else if(wParam == VK_ESCAPE && Console::IsVisible())
		{
			if(isUp)
				Console::Hide();
			return 1;
		}
		else if(Console::IsEnabled())
		{
			BYTE layout[256] = {0};
			WORD out[2] = {0};
			switch(wParam)
			{
				case VK_TAB:
					if(isUp)
						for(int i = 0; i < 5; i++)
							Console::AddKey(' ');
					break;
				case VK_RETURN:
					if(isUp && !isRepeat && !escMenuOpen)
						Console::ExecuteCommand();
					break;
				case VK_BACK:
					if(isDown)
						Console::RemoveLastKey();
					break;
				case VK_UP:
					if(isUp && !isRepeat)
						Console::PrevCommand();
					break;
				case VK_DOWN:
					if(isUp && !isRepeat)
						Console::NextCommand();
					break;
				default:
					if(isDown)
					{
						GetKeyboardState(layout);
						if(ToAscii(wParam, (lParam & 0xFF0000), layout, out, 0) != 0)
						{
							for(int i = 0; i < repeatCount; i++)
								Console::AddKey(out[0]);
						}
					}
					break;
			}
			return 1;
		}
		else if(!(chatBoxOpen || escMenuOpen) && !isRepeat && code == HC_ACTION)
			KeyDownUpEvent(wParam, isUp);
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	if(Vars.bBlockMouse)
		return 1;

	if(code == HC_ACTION)
	{
		MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*)lParam;
		POINT pt = mouse->pt;
		ScreenToClient(mouse->hwnd, &pt);
		bool clicked = false;

		switch(wParam)
		{
			case WM_LBUTTONDOWN:
				MouseClickEvent(0, pt, false);
				for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
					if((*it)->Click(0, &pt))
						clicked = true;
				break;
			case WM_LBUTTONUP:
				MouseClickEvent(0, pt, true);
				break;
			case WM_RBUTTONDOWN:
				MouseClickEvent(1, pt, false);
				for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
					if((*it)->Click(1, &pt))
						clicked = true;
				break;
			case WM_RBUTTONUP:
				MouseClickEvent(1, pt, true);
				break;
			case WM_MBUTTONDOWN:
				MouseClickEvent(2, pt, false);
				for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
					if((*it)->Click(2, &pt))
						clicked = true;
				break;
			case WM_MBUTTONUP:
				MouseClickEvent(2, pt, true);
				break;
			case WM_MOUSEMOVE:
	//			MouseMoveEvent(pt);
	//			for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
	//				(*it)->Hover(&pt);
				break;
		}

		return clicked ? 1 : CallNextHookEx(NULL, code, wParam, lParam);
	}

	//return clicked ? 1 : CallNextHookEx(Vars.hMouseHook, code, wParam, lParam);
	return CallNextHookEx(NULL, code, wParam, lParam);
}

VOID GameDraw(VOID)
{
	Console::Draw();
	if(ClientState() != ClientStateMenu)
		Genhook::DrawAll(IG);
}

VOID GameDrawOOG(VOID)
{
	D2WIN_DrawSprites();
	Console::Draw();
	if(ClientState() == ClientStateMenu)
		Genhook::DrawAll(OOG);
}

VOID __fastcall WhisperHandler(CHAR* szAcc, CHAR* szText)
{
	WhisperEvent(szAcc, szText);
}

DWORD __fastcall GameAttack(AttackStruct* pAttack)
{
	if(!pAttack || !pAttack->lpTargetUnit || pAttack->lpTargetUnit->dwType != UNIT_MONSTER)
		return (DWORD)-1;

	if(pAttack->dwAttackType == ATTACKTYPE_UNITLEFT)
		pAttack->dwAttackType = ATTACKTYPE_SHIFTLEFT;

	if(pAttack->dwAttackType == ATTACKTYPE_RIGHT)
		pAttack->dwAttackType = ATTACKTYPE_SHIFTRIGHT;

	return NULL;
}

VOID __fastcall GamePlayerAssignment(UnitAny* pPlayer)
{
	if(!pPlayer)
		return;

	PlayerAssignEvent(pPlayer->dwUnitId);
}

