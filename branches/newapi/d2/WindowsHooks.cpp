#include <windows.h>

#include "WindowsHooks.h"
#include "D2Events.h"
#include "D2Utilities.h"
#include "Functions.h"

#include "Script.h"

#define COPYDATA_EXEC_SCRIPT 0x1337

// TODO: Move these globals somewhere else
extern HHOOK hKeybHook, hMouseHook;
extern WNDPROC oldWndProc;
extern bool bBlockMinimize;

LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam)
{
	if(code != HC_ACTION)
		return CallNextHookEx(hKeybHook, code, wParam, lParam);

	bool pressed = false;
	// ignore key events if the key is a repeat
	if(!(!!(lParam & 0x40000000) == 1 && !!(lParam & 0x80000000) == 0))
	{
		// ignore key events if the chatbox or the esc menu are open
		if(GameReady())
		{
			if(!(GetUIState(5) || GetUIState(9)))
				pressed = KeyPressEvent(wParam, !!(lParam & 0x80000000));
		}
		else
			pressed = KeyPressEvent(wParam, !!(lParam & 0x80000000));
	}
	
	return pressed ? 1 : CallNextHookEx(hKeybHook, code, wParam, lParam);
}


LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam)
{
	if(code != HC_ACTION)
		return CallNextHookEx(hMouseHook, code, wParam, lParam);

	MOUSEHOOKSTRUCT* mouse = (MOUSEHOOKSTRUCT*)lParam;
	POINT pt = mouse->pt;
	ScreenToClient(mouse->hwnd, &pt);
	bool clicked = false;

	switch(wParam)
	{
		case WM_LBUTTONDOWN:
			clicked = MouseClickEvent(0, pt, false);
			break;
		case WM_LBUTTONUP:
			clicked = MouseClickEvent(0, pt, true);
			break;
		case WM_RBUTTONDOWN:
			clicked = MouseClickEvent(1, pt, false);
			break;
		case WM_RBUTTONUP:
			clicked = MouseClickEvent(1, pt, true);
			break;
		case WM_MBUTTONDOWN:
			clicked = MouseClickEvent(2, pt, false);
			break;
		case WM_MBUTTONUP:
			clicked = MouseClickEvent(2, pt, true);
			break;
		case WM_MOUSEMOVE:
			MouseMoveEvent(pt);
			break;
	}

	return clicked ? 1 : CallNextHookEx(hMouseHook, code, wParam, lParam);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COPYDATA:
			if(lParam)
			{
				COPYDATASTRUCT* pCopy = (COPYDATASTRUCT*)lParam;
				if(pCopy->dwData == COPYDATA_EXEC_SCRIPT)
				{
					Script* script = Script::CompileCommand((char*)pCopy->lpData);
					if(script)
						script->Start();
				}
				else
					CopyDataEvent(pCopy->dwData, (char*)pCopy->lpData);
			}
			return TRUE;
			break;
		case WM_ACTIVATEAPP:
			if(!wParam && bBlockMinimize)
				return NULL;
			break;

	}

	return CallWindowProcA(oldWndProc, hWnd, uMsg, wParam, lParam);
}
