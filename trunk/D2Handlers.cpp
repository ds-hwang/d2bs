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
	BOOL bInGame = TRUE;
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

		if(GameReady())
		{
			Vars.image->SetX(D2GetScreenSizeX()/2);
			Vars.text->SetX(D2GetScreenSizeX()/2);

			if(bInGame && ((Vars.dwMaxGameTime > 0 && Vars.dwGameTime > 0 && (GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime) ||
				(!IsTownLevel(GetPlayerArea()) && (Vars.nChickenHP > 0 && Vars.nChickenHP >= GetUnitHP(D2CLIENT_GetPlayerUnit())) || (Vars.nChickenMP > 0 && Vars.nChickenMP >= GetUnitMP(D2CLIENT_GetPlayerUnit())))))
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
		else if(!D2CLIENT_GetPlayerUnit() && *p_D2WIN_FirstControl)
		{
			Vars.image->SetX(D2GetScreenSizeX()/2);
			Vars.text->SetX(D2GetScreenSizeX()/2);

			if(!bClicked)
			{
				if(Vars.bStartAtMenu)
					clickControl(*p_D2WIN_FirstControl);
				bClicked = TRUE;
			}
			
			if(bInGame)
			{
				Vars.dwGameTime = NULL;
				bInGame = FALSE;

				if(!bStarterScript)
				{
					Script* script = ScriptEngine::CompileFile(starterdbj, OutOfGame);
					if(script)
						CreateThread(0, 0, ScriptThread, script, 0, 0);
					else
						Print("ÿc2D2BSÿc0 :: Failed to start starter.dbj!");
					bStarterScript = TRUE;
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
		else if(!_strcmpi(argv[0], "stop"))
		{
			if(ScriptEngine::GetCount() > 0)
				Print("ÿc2D2BSÿc0 :: Stopping all scripts!");

			ScriptEngine::StopAll(true);
			result = -1;
		}
		else if(!_strcmpi(argv[0], "reload"))
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
		else if(!_strcmpi(argv[0], "flush"))
		{
			if(!Vars.bDisableCache)
			{
				Print("ÿc2D2BSÿc0 :: Flushing the script cache...");
				ScriptEngine::FlushCache();
			}
			result = -1;
		}
		else if(!_strcmpi(argv[0], "exec"))
		{
			if(argc >= 2)
			{
				Script* script = ScriptEngine::CompileCommand(szBuffer+5);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);
			}

			result = -1;
		}
		else if(!_strcmpi(argv[0], "load"))
		{
			if(argc >= 2)
			{
				Print("ÿc2D2BSÿc0 :: Loading %s", argv[1]);

				CHAR szPath[8192] = "";
				sprintf(szPath, "%s\\%s", Vars.szScriptPath, argv[1]);

				Script* script = ScriptEngine::CompileFile(szPath, InGame, true);
				if(script)
					CreateThread(0, 0, ScriptThread, script, 0, 0);
				else
					Print("ÿc2D2BSÿc0 :: Failed to load %s!", argv[1]);
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
	if((code != HC_ACTION))
		return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);

	if(Vars.bBlockKeys)
		return 1;

	if(wParam == 0xFF)
		return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);

	// ignore key events if the key is a repeat
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

	if(wParam == VK_HOME && !altState && isUp && !(chatBoxOpen || escMenuOpen))
	{
		Console::Toggle();
		return 1;
	}
	else if(Console::IsVisible())
	{
		if(wParam == VK_ESCAPE && !isRepeat)
		{
			if(isUp)
				Console::Toggle();
			return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);
		}
		else if(wParam == VK_RETURN && !isRepeat && !escMenuOpen)
		{
			if(isUp)
				Console::ExecuteCommand();
			return 1;
		}
		else if(wParam == VK_BACK)
		{
			if(isUp)
				Console::RemoveLastKey();
			return 1;
		}
		else if(isDown)
		{
			BYTE layout[256];
			GetKeyboardState(layout);
			WORD out[2];
			if(ToAscii(wParam, (lParam & 0xFF0000), layout, out, 0) != 0)
			{
				for(int i = 0; i < repeatCount; i++)
					Console::AddKey(out[0]);
				return 1;
			}
		}
	}
	else if(!(chatBoxOpen || escMenuOpen) && !isRepeat)
			KeyDownUpEvent(wParam, transitionState);
	
	return CallNextHookEx(Vars.hKeybHook, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	if((code != HC_ACTION))
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
	if(!Console::IsReady())
		Console::Initialize();
	Console::Draw();
	if(GameReady())
		Genhook::DrawAll(IG);
}

VOID GameDrawOOG(VOID)
{
	D2WIN_DrawSprites();
	if(!Console::IsReady())
		Console::Initialize();
	Console::Draw();
	if(!GameReady())
		Genhook::DrawAll(OOG);
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

