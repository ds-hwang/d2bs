#include <vector>

#include "D2Helpers.h"
#include "Helpers.h"
#include "ScriptEngine.h"
#include "D2Ptrs.h"
#include "D2NetHandlers.h"
#include "Events.h"
#include "Constants.h"
#include "Console.h"
#include "ScreenHook.h"

using namespace std;

DWORD WINAPI D2Thread(LPVOID lpParam)
{
	bool beginStarter = true;
	bool bInGame = false;
	InitSettings();
	if(InitHooks())
	{
		Log("D2BS Engine startup complete.");
		Print("ÿc2D2BSÿc0 :: Engine startup complete!");
	}
	else
	{
		Log("D2BS Engine startup failed.");
		Print("ÿc2D2BSÿc0 :: Engine startup failed!");
		return FALSE;
	}

	ClientGameState oldState = ClientStateNull;
	while(Vars.bActive)
	{
		// TODO look at this more
		ClientGameState state = ClientState();
		if((state == ClientStateInGame || state == ClientStateMenu) && oldState != state)
		{
			// we switched from in game to out or something
			JS_TriggerAllOperationCallbacks(ScriptEngine::GetRuntime());
			oldState = state;

			switch(state)
			{
			case ClientStateInGame:
				{
					if(bInGame)
					{
						if((Vars.dwMaxGameTime && Vars.dwGameTime && 
							(GetTickCount() - Vars.dwGameTime) > Vars.dwMaxGameTime))
							D2CLIENT_ExitGame();
					}
					else
					{
						Vars.dwGameTime = GetTickCount();
						D2CLIENT_InitInventory();

						GameJoined();

						bInGame = true;
					}
					break;
				}
			case ClientStateMenu:
				{
					MenuEntered(beginStarter);
					beginStarter = false;
					if(bInGame)
					{
						Vars.dwGameTime = 0;
						bInGame = false;
					}
					break;
				}
			default:
				break;
			}
		}

		Sleep(50);
	}

	ScriptEngine::Shutdown();

	return NULL;
}

DWORD __fastcall GameInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
	{
		Vars.bDontCatchNextMsg = FALSE;
		return 0;
	}

	bool result = false;

	if(wMsg[0] == L'.')
	{
		char* szBuffer = UnicodeToAnsi(wMsg);
		result = ProcessCommand(szBuffer+1, false);
		delete[] szBuffer;
	}

	return result == true ? -1 : 0;
}

DWORD __fastcall ChannelInput(wchar_t* wMsg)
{
	if(Vars.bDontCatchNextMsg)
	{
		Vars.bDontCatchNextMsg = FALSE;
		return false;
	}

	bool result = false;
	if(wMsg[0] == L'.')
	{
		char* szBuffer = UnicodeToAnsi(wMsg);
		result = ProcessCommand(szBuffer+1, false);
		// TODO: Clear the msg buffer, this DOES NOT clear it
		//wMsg = L"";
		delete[] szBuffer;
	}

	return result;
}

DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize)
{
	switch(pPacket[0])
	{
		case 0x15: return ReassignPlayerHandler(pPacket, dwSize);
		case 0x26: return ChatEventHandler(pPacket, dwSize);
		case 0x2A: return NPCTransactionHandler(pPacket, dwSize);
		case 0x5A: return EventMessagesHandler(pPacket, dwSize);
		case 0x18:
		case 0x95: return HPMPUpdateHandler(pPacket, dwSize);
		case 0x9C:
		case 0x9D: return ItemActionHandler(pPacket, dwSize);
		// TODO: remove this when we get real a-d
		//case 0xAE: TerminateProcess(GetCurrentProcess(), 0); break;
		case 0xA7: return DelayedStateHandler(pPacket, dwSize);
	}

	return 1;
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
	}

	return (LONG)CallWindowProcA(Vars.oldWNDPROC, hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(code >= HC_ACTION)
	{
		WORD repeatCount = LOWORD(lParam);
		bool altState = !!(HIWORD(lParam) & KF_ALTDOWN);
		bool previousState = !!(HIWORD(lParam) & KF_REPEAT);
		bool transitionState = !!(HIWORD(lParam) & KF_UP);
		bool isRepeat = !transitionState && repeatCount != 1;
		bool isDown = !(previousState && transitionState);
		bool isUp = previousState && transitionState;

		bool gameState = ClientState() == ClientStateInGame;
		bool chatBoxOpen = gameState ? !!D2CLIENT_GetUIState(UI_CHAT_CONSOLE) : false;
		bool escMenuOpen = gameState ? !!D2CLIENT_GetUIState(UI_ESCMENU_MAIN) : false;

		if(altState && wParam == VK_F4)
			return CallNextHookEx(NULL, code, wParam, lParam);

		if(Vars.bBlockKeys)
			return 1;
		
		if(wParam == VK_HOME && !(chatBoxOpen || escMenuOpen))
		{
			if(isDown && !isRepeat && code == HC_ACTION)
			{
				if(!altState)
					Console::ToggleBuffer();
				else
					Console::TogglePrompt();

				return CallNextHookEx(NULL, code, wParam, lParam);
			}
		}
		else if(wParam == VK_ESCAPE && Console::IsVisible())
		{
			if(isDown && !isRepeat && code == HC_ACTION )
			{
				Console::Hide();
				return 1;
			}
			return CallNextHookEx(NULL, code, wParam, lParam);
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
		else if(code == HC_ACTION && !isRepeat && !(chatBoxOpen || escMenuOpen))
			KeyDownUpEvent(wParam, isUp);
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*)lParam;
	POINT pt = mouse->pt;
	ScreenToClient(mouse->hwnd, &pt);

	// filter out clicks on the window border
	if(code == HC_ACTION && (pt.x < 0 || pt.y < 0))
		return CallNextHookEx(NULL, code, wParam, lParam);

	if(Vars.bBlockMouse)
		return 1;

	if(code == HC_ACTION)
	{
		bool clicked = false;

		switch(wParam)
		{
			case WM_LBUTTONDOWN:
				MouseClickEvent(0, pt, false);
				break;
			case WM_LBUTTONUP:
				MouseClickEvent(0, pt, true);
				break;
			case WM_RBUTTONDOWN:
				MouseClickEvent(1, pt, false);
				break;
			case WM_RBUTTONUP:
				MouseClickEvent(1, pt, true);
				break;
			case WM_MBUTTONDOWN:
				MouseClickEvent(2, pt, false);
				break;
			case WM_MBUTTONUP:
				MouseClickEvent(2, pt, true);
				break;
			case WM_MOUSEMOVE:
				// would be nice to enable these events but they bog down too much
				//MouseMoveEvent(pt);
				//Genhook::ForEachVisibleHook(HoverHook, &helper, 1);
				break;
		}

		return clicked ? 1 : CallNextHookEx(NULL, code, wParam, lParam);
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

void GameDraw(void)
{
	if(Vars.bActive && ClientState() == ClientStateInGame)
	{
		DrawLogo();
		Console::Draw();
	}
	Sleep(1);
}

void GameDrawOOG(void)
{
	D2WIN_DrawSprites();
	if(Vars.bActive && ClientState() == ClientStateMenu)
	{
		DrawLogo();
		Console::Draw();
	}
	Sleep(10);
}

void __stdcall AddUnit(UnitAny* lpUnit)
{
	// TODO: build a JSObject around this unit, JS_AddRoot it, and push it onto a list
}

void __stdcall RemoveUnit(UnitAny* lpUnit)
{
	// TODO: JS_RemoveRoot the JSObject this unit represents and set its private data to null... maybe?
}

void __fastcall WhisperHandler(char* szAcc, char* szText)
{
	WhisperEvent(szAcc, szText);
}

void __fastcall ChannelWhisperHandler(char* szAcc, char* szText)
{
	WhisperEvent(szAcc, szText);
}

void __fastcall ChannelChatHandler(char* szAcc, char* szText)
{
	ChatEvent(szAcc, szText);
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

void __fastcall GamePlayerAssignment(UnitAny* pPlayer)
{
	if(!pPlayer)
		return;

	PlayerAssignEvent(pPlayer->dwUnitId);
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	if(Vars.bGameLoopEntered)
		LeaveCriticalSection(&Vars.cGameLoopSection);
	else
		Vars.bGameLoopEntered = true;

	while(Vars.SectionCount)
		Sleep(0);

	EnterCriticalSection(&Vars.cGameLoopSection);	
}


void GameLeave(void)
{
	if(Vars.bGameLoopEntered)
		LeaveCriticalSection(&Vars.cGameLoopSection);
	else
		Vars.bGameLoopEntered = true;

	EnterCriticalSection(&Vars.cGameLoopSection);
}