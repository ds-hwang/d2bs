#include <vector>

#include "D2Handlers.h"
#include "D2BS.h"
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

#include "debugnew/debug_new.h"

using namespace std;

Variables Vars;

DWORD WINAPI D2Thread(LPVOID lpParam)
{
	BOOL bInGame = FALSE;
	BOOL bStarterScript = FALSE;
	BOOL bClicked = FALSE;
	Vars.dwGameTime = GetTickCount();

	// calculate the path to starter/default.dbj only once
	char defaultdbj[_MAX_PATH+_MAX_FNAME];
	sprintf(defaultdbj, "%s\\default.dbj", Vars.szScriptPath);
	char starterdbj[_MAX_PATH+_MAX_FNAME];
	sprintf(starterdbj, "%s\\starter.dbj", Vars.szScriptPath);

	while(Vars.bActive)
	{
		if(!Vars.oldWNDPROC && D2WIN_GetHwnd())
			Vars.oldWNDPROC = (WNDPROC)SetWindowLong(D2WIN_GetHwnd(), GWL_WNDPROC, (LONG)GameEventHandler);

		switch(GetClientState())
		{
			case ClientStateReady:
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
			//case ClientStateBusy:
			case ClientStateOOG:
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
						clickControl(*p_D2WIN_FirstControl);
					bClicked = TRUE;
				}

				if(!bStarterScript && Vars.oldWNDPROC) // Vars.oldWNDPROC makes this debug mode friendly 
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

	return NULL;
}

DWORD __fastcall GameInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
		return NULL;

	char* szBuffer = UnicodeToAnsi(wMsg);
	int result = 0;

	if(szBuffer[0] == '.')
	{
		char* buf = _strdup(szBuffer);
		char* cmd = strtok(buf+1, " ");

		if(!_strcmpi(cmd, "start"))
		{
			char file[_MAX_PATH+_MAX_FNAME];
			sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
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
			sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
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
				sprintf(szPath, "%s\\%s", Vars.szScriptPath, arg);

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
		char Code[5];
		WORD itemX;
		WORD itemY;
		//(data+pos/8)<<(64-len-(pos&7))>>(64-len)); taken from magnet and mousepad
		//		date=packet, len= size of data being red, pos = where in the packet -1
	
		Code[0]=(*(unsigned __int64 *)(pPacket+141/8)<<(64-8-(141&7))>>(64-8));		
		Code[1]=(*(unsigned __int64 *)(pPacket+149/8)<<(64-8-(149&7))>>(64-8));	
		Code[2]=(*(unsigned __int64 *)(pPacket+157/8)<<(64-8-(157&7))>>(64-8));	
		Code[3]=(*(unsigned __int64 *)(pPacket+165/8)<<(64-8-(165&7))>>(64-8));	
		if(Code[3] == ' ') {  //terminate the string
			Code[3] = '\0';
		} else {
			Code[4] = '\0';
		}
		itemX=(*(unsigned __int64 *)(pPacket+108/8)<<(64-16-(108&7))>>(64-16));	
		itemY=(*(unsigned __int64 *)(pPacket+125/8)<<(64-16-(125&7))>>(64-16));	
		itemX=itemX/2;
		//itemY=itemY/2; //only x gets /2
		WORD Mode = *(BYTE*)&pPacket[1];
		DWORD GID = *(DWORD*)&pPacket[4];
		BYTE Type = *(BYTE*)&pPacket[3];
		CHAR* ItemCode = Code;
		
		ItemDropEvent(GID,Code,itemX,itemY,Mode );
		}
	}
	else if(pPacket[0] == 0x5a){ // SOJ and Walks Msg by bobite
		if (pPacket[1] == 0x11){ //stones
			DWORD soj = *(DWORD*)&pPacket[3];
			char mess[256]; sprintf(mess, "%u Stones of Jordan Sold to Merchants", soj);				
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
			break;
		case WM_ACTIVATEAPP:
			if(!wParam && Vars.bBlockMinimize)
				return NULL;
			break;
	}

	return (LONG)CallWindowProcA(Vars.oldWNDPROC, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(code < 0)
		return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);

	if(Vars.bBlockKeys)
		return 1;

	if(wParam == 0xFF)
		return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);

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
	else if(Console::IsVisible())
	{
		BYTE layout[256];
		WORD out[2];
		switch(wParam)
		{
			case VK_TAB:
				if(isUp && Console::IsEnabled())
					for(int i = 0; i < 5; i++)
						Console::AddKey(' ');
				break;
			case VK_ESCAPE:
				if(isUp)
					Console::Hide();
				break;
			case VK_RETURN:
				if(isUp && !isRepeat && !escMenuOpen && Console::IsEnabled())
					Console::ExecuteCommand();
				break;
			case VK_BACK:
				if(isDown && Console::IsEnabled())
					Console::RemoveLastKey();
				break;
			case VK_UP:
				if(isUp && !isRepeat && Console::IsEnabled())
					Console::PrevCommand();
				break;
			case VK_DOWN:
				if(isUp && !isRepeat && Console::IsEnabled())
					Console::NextCommand();
				break;
			default:
				if(isDown && Console::IsEnabled())
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
	else if(!(chatBoxOpen || escMenuOpen) && !isRepeat)
		KeyDownUpEvent(wParam, transitionState);
	
	return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	if(code < 0)
		return CallNextHookEx(Vars.hMouseHook, code, wParam, lParam);

	if(Vars.bBlockMouse)
		return 1;

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
			MouseMoveEvent(pt);
			for(HookList::iterator it = Genhook::GetFirstHook(); it != Genhook::GetLastHook(); it++)
				(*it)->Hover(&pt);
			break;
	}

	return clicked ? 1 : CallNextHookEx(Vars.hMouseHook, code, wParam, lParam);
}

VOID GameDraw(VOID)
{
	Console::Draw();
	if(GetClientState() != ClientStateOOG)
		Genhook::DrawAll(IG);
}

VOID GameDrawOOG(VOID)
{
	D2WIN_DrawSprites();
	Console::Draw();
	if(GetClientState() == ClientStateOOG)
		Genhook::DrawAll(OOG);
}

VOID __fastcall WhisperHandler(CHAR* szAcc, CHAR* szText)
{
	WhisperEvent(szAcc, szText);
}

DWORD __fastcall GameAttack(AttackStruct* pAttack)
{
	if(!pAttack)
		return -1;

	if(!pAttack->lpTargetUnit)
		return -1;

	if(pAttack->lpTargetUnit->dwType != UNIT_MONSTER)
		return -1;

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

