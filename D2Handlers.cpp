#include "D2BS.h"
#include "Script.h"
#include <vector>
#include "Unit.h"

using namespace std;

DWORD WINAPI D2Thread(LPVOID lpParam)
{
	BOOL bInGame = TRUE;
	BOOL bStarterScript = FALSE;
	Vars.dwGameTime = GetTickCount();
	Genhook *text, *image;

	while(TRUE)
	{
		if(!Vars.oldWNDPROC && D2WIN_GetHwnd())
		{
			Vars.oldWNDPROC = (WNDPROC)SetWindowLong(D2WIN_GetHwnd(),GWL_WNDPROC,(LONG)GameEventHandler);

			char szVersionString[64] = "";
			sprintf(szVersionString, "D2BS %s",D2BS_VERSION);
			image = new ImageHook(NULL, "version.bmp", D2GetScreenSizeX()/2, 10, false, 0, 0, Center, Perm);
			text = new TextHook(NULL, szVersionString, D2GetScreenSizeX()/2, 15, false, 13, 4, 0, Center, Perm);
		}

		if(GameReady())
		{
			image->SetX(D2GetScreenSizeX()/2);
			text->SetX(D2GetScreenSizeX()/2);

			if(bInGame && ((Vars.dwMaxGameTime > 0 && Vars.dwGameTime > 0 && (GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime) ||
				(!IsTownLevel(GetPlayerArea()) && (Vars.nChickenHP > 0 && Vars.nChickenHP >= GetUnitHP(D2CLIENT_GetPlayerUnit())) || (Vars.nChickenMP > 0 && Vars.nChickenMP >= GetUnitMP(D2CLIENT_GetPlayerUnit())))))
					D2CLIENT_ExitGame();

			if(!bInGame)
			{
				Sleep(500);

				Vars.dwGameTime = GetTickCount();
				D2CLIENT_InitInventory();

				Print("D2BS: Starting default.dbj");
				char file[_MAX_PATH+_MAX_FNAME];
				sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
				Script* script = Script::CompileFile(file, InGame);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);

				bInGame = TRUE;
			}
		}
		else if(!D2CLIENT_GetPlayerUnit() && *p_D2WIN_FirstControl)
		{
			
			
			image->SetX(D2GetScreenSizeX()/2);
			text->SetX(D2GetScreenSizeX()/2);
			
			if(bInGame)
			{
				Vars.dwGameTime = NULL;
				bInGame = FALSE;

				if(!bStarterScript)
				{
					
					char file[_MAX_PATH+_MAX_FNAME];
					sprintf(file, "%s\\starter.dbj", Vars.szScriptPath);
					if(_access(file, 0) == 0) {

						clickControl((Control*)*p_D2WIN_FirstControl);
						Script* script = Script::CompileFile(file, OutOfGame);
						if(script)
							CreateThread(0, 0, ScriptThread, script, 0, 0);
						bStarterScript = TRUE;
					}
				}
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
	char *argv[0x10];
	int result = 0;

	if(szBuffer[0] == '.')
	{
		INT argc = StringTokenize(szBuffer + 1, ' ', argv, 0x10);

		if(!_strcmpi(argv[0], "start"))
		{
			Print("D2BS: Starting default.dbj!");

			char file[_MAX_PATH+_MAX_FNAME];
			sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
			Script* script = Script::CompileFile(file, InGame);
			if(script)
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			result = -1;
		}
		else if(!_strcmpi(argv[0], "stop"))
		{
			if(Script::GetActiveCount() > 0)
				Print("D2BS: Stopping all scripts!");

			Script::StopAll();
			result = -1;
		}
		else if(!_strcmpi(argv[0], "reload"))
		{
			if(Script::GetActiveCount() > 0)
				Print("D2BS: Stopping all scripts...");
			Script::StopAll();

			Print("D2BS: Flushing the script cache...");
			Script::FlushCache();

			Print("D2BS: Starting default.dbj...");
			char file[_MAX_PATH+_MAX_FNAME];
			sprintf(file, "%s\\default.dbj", Vars.szScriptPath);
			Script* script = Script::CompileFile(file, InGame);
			if(script)
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			result = -1;
		}
		else if(!_strcmpi(argv[0], "flush"))
		{
			Print("D2BS: Flushing the script cache...");
			Script::FlushCache();
			result = -1;
		}
		else if(!_strcmpi(argv[0], "exec"))
		{
			if(argc >= 2)
			{
				Script* script = Script::CompileCommand(szBuffer+5);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);
			}

			result = -1;
		}
		else if(!_strcmpi(argv[0], "load"))
		{
			if(argc >= 2)
			{
				Print("D2BS: Loading %s", argv[1]);

				CHAR szPath[8192] = "";
				sprintf(szPath, "%s\\%s", Vars.szScriptPath, argv[1]);

				Script* script = Script::CompileFile(szPath, InGame, true);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);
			}

			result = -1;
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
					Script* script = Script::CompileCommand((char*)pCopy->lpData);
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
	// ignore key events if the key is a repeat
	if((code == HC_ACTION) && !(!!(lParam & 0x40000000) == 1 && !!(lParam & 0x80000000) == 0))
	{
		if(GameReady())
		{
			// ignore key events if the chatbox or the esc menu are open
			if(!(D2CLIENT_GetUIState(5) || D2CLIENT_GetUIState(9)))
				KeyDownUpEvent(wParam, !!(lParam & 0x80000000));
		}
		else
			KeyDownUpEvent(wParam, !!(lParam & 0x80000000));
	}
	
	return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);
}


LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	if((code != HC_ACTION))
		return CallNextHookEx(Vars.hMouseHook, code, wParam, lParam);
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
	if(GameReady())
		Genhook::DrawAll(IG);
}

VOID GameDrawOOG(VOID)
{
	D2WIN_DrawSprites();
	if(!GameReady())
		Genhook::DrawAll(OOG);
	GameDraw();
}

VOID __fastcall WhisperHandler(CHAR* szAcc, CHAR* szText)
{
	BNCSChatEvent(szAcc, szText);
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
