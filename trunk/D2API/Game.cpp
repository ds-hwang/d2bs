#include <string>

#include "Game.h"
#include "D2Ptrs.h"
#include "Constants.h"
#include "Control.h"
#include "Helpers.h"
#include "D2Helpers.h"

BOOL g_DontCatchNextMsg;
BOOL g_ClickAction;
DWORD g_SelectedUnitId;
DWORD g_SelectedUnitType;

void SendGold(int nGold, int nMode)
{
	*p_D2CLIENT_GoldDialogAmount = nGold;
	*p_D2CLIENT_GoldDialogAction = nMode;
	D2CLIENT_PerformGoldDialogAction();
}

void __declspec(naked) __fastcall Say_ASM(DWORD dwPtr)
{
	__asm
	{
		POP EAX
		PUSH ECX
		PUSH EAX
		SUB ESP,0x110
		PUSH EBX
		PUSH EBP
		MOV EBP, [D2LANG_Say_II]
		PUSH ESI
		PUSH EDI
		JMP D2CLIENT_Say_I
	}
}

void Say(const char *szMessage, ...) 
{ 
	char szBuffer[255] = {0};
	va_list vaArgs;
	va_start(vaArgs, szMessage);
	vsprintf_s(szBuffer, sizeof(szBuffer), szMessage, vaArgs);
	va_end(vaArgs);
	g_DontCatchNextMsg = TRUE;

	if(D2CLIENT_GetPlayerUnit())
	{
		wchar_t* wBuffer = AnsiToUnicode(szBuffer);
		memcpy((wchar_t*)p_D2CLIENT_ChatMsg, wBuffer, wcslen(wBuffer)*2+1);
		delete[] wBuffer;
		wBuffer = NULL;

		MSG* aMsg = new MSG;
		aMsg->hwnd = D2GFX_GetHwnd();
		aMsg->message = WM_CHAR;
		aMsg->wParam = VK_RETURN;
		aMsg->lParam = 0x11C0001;
		aMsg->time = NULL;
		aMsg->pt.x = 0x79;
		aMsg->pt.y = 0x1;

		Say_ASM((DWORD)&aMsg);
		delete aMsg;
		aMsg = NULL;
	}
	else if(OOG_GetLocation() == OOG_CHANNEL)
	{
		memcpy((char*)p_D2MULTI_ChatBoxMsg, szBuffer, strlen(szBuffer) + 1);
		D2MULTI_DoChat();
	}
}

bool ClickMap(DWORD dwClickType, WORD wX, WORD wY, BOOL bShift, UnitAny* pUnit)
{
	if(ClientState() != ClientStateInGame)
		return false;

	if((wX < 0xFFFF && wY < 0xFFFF && pUnit) || ((wX == 0xFFFF || wY == 0xFFFF) && !pUnit))
	{
#ifdef DEBUG
		DebugBreak();
#endif
		return false;
	}

	POINT Click = {wX, wY};
	if(pUnit)
	{
		Click.x = D2CLIENT_GetUnitX(pUnit);
		Click.y = D2CLIENT_GetUnitY(pUnit);
	}

	D2COMMON_MapToAbsScreen(&Click.x, &Click.y);

	Click.x -= *p_D2CLIENT_MouseOffsetX;
	Click.y -= *p_D2CLIENT_MouseOffsetY;

	POINT OldMouse = {0, 0};
	OldMouse.x = *p_D2CLIENT_MouseX;
	OldMouse.y = *p_D2CLIENT_MouseY;
	*p_D2CLIENT_MouseX = 0;
	*p_D2CLIENT_MouseY = 0;

	if(pUnit && pUnit != D2CLIENT_GetPlayerUnit())
	{
		g_SelectedUnitId = pUnit->dwUnitId;
		g_SelectedUnitType = pUnit->dwType;

		g_ClickAction = TRUE;

		D2CLIENT_ClickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : 0x08);
		D2CLIENT_SetSelectedUnit(NULL);

		g_ClickAction = FALSE;
		g_SelectedUnitId = NULL;
		g_SelectedUnitType = NULL;
	}
	else
	{
		g_SelectedUnitId = NULL;
		g_SelectedUnitType = NULL;

		g_ClickAction = TRUE;
		D2CLIENT_ClickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : 8);
		g_ClickAction = FALSE;
	}

	*p_D2CLIENT_MouseX = OldMouse.x;
	*p_D2CLIENT_MouseY = OldMouse.y;
	return TRUE;
}
