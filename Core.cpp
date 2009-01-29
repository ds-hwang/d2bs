#include "D2BS.h"

VOID Print(CHAR* szFormat, ...)
{
	char* szString = NULL;
	va_list vaArgs;

	va_start(vaArgs, szFormat);
	int len = _vscprintf(szFormat, vaArgs);
	szString = new char[len+1];
	vsprintf_s(szString, len+1, szFormat, vaArgs);
	va_end(vaArgs);

#define MAXLEN 500
	if(len > MAXLEN)
	{
		for(int i = 0; i < (len/MAXLEN)+1; i++)
		{
			char tmp[MAXLEN+1] = "";
			memcpy(tmp, szString+i*MAXLEN, MAXLEN);
			Print(tmp);
		}
		delete[] szString;
		return;
	}
#undef MAXLEN

	EnterCriticalSection(&Vars.cPrintSection);
	if(D2CLIENT_GetPlayerUnit() && GameReady())
	{
		wchar_t* wOutput = AnsiToUnicode(szString);
		// the 200 character limit seems to have been lifted... longer than 200 characters didn't crash
		D2CLIENT_PrintGameString(wOutput, 0);
		delete[] wOutput;
		// TODO: Fix this detection to figure out if we're in a channel or not
	} else if(false) {
		D2MULTI_PrintChannelText(szString, NULL);
	} else {
		MessageBox(0, szString, "D2BS " D2BS_VERSION, 0);
	}
	LeaveCriticalSection(&Vars.cPrintSection);
	delete[] szString;
}

VOID __declspec(naked) __fastcall Say_ASM(DWORD dwPtr)
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

VOID Say(char *szMessage, ...) 
{ 
	CHAR szBuffer[8192] = {0};
	va_list vaArgs;
	va_start(vaArgs, szMessage);
	vsprintf(szBuffer, szMessage, vaArgs);
	va_end(vaArgs);


	if(D2CLIENT_GetPlayerUnit())
	{
		wchar_t* wBuffer = AnsiToUnicode(szBuffer);
		memcpy((wchar_t*)p_D2CLIENT_ChatTextBuffer, wBuffer, wcslen(wBuffer)*2+1);

		MSG aMsg = {D2WIN_GetHwnd(), WM_CHAR, VK_RETURN, 0x11C0001, 0, {0x79, 0x1}};		
		Say_ASM((DWORD)&aMsg);
		delete[] wBuffer;
	}
	else
	{
		memcpy((CHAR*)p_D2MULTI_ChatBoxMsg, szBuffer, strlen(szBuffer) + 1);
		D2MULTI_DoChat();
	}
}

BOOL ClickMap(DWORD dwClickType, WORD wX, WORD wY, BOOL bShift, UnitAny* pUnit)
{
	DWORD dwUnitId = NULL;
	DWORD dwUnitType = NULL;

	if(pUnit)
	{
		dwUnitId = pUnit->dwUnitId;
		dwUnitType = pUnit->dwType;
	}

	CriticalMisc cMisc;
	cMisc.EnterSection();

	if(dwUnitId)
	{
		pUnit = D2CLIENT_FindUnit(dwUnitId, dwUnitType);

		if(!pUnit)
			return FALSE;

		Vars.dwSelectedUnitId = dwUnitId;
		Vars.dwSelectedUnitType = dwUnitType;

		POINT Click = {wX, wY};

		D2COMMON_MapToAbsScreen(&Click.x, &Click.y);

		Click.x -= *p_D2CLIENT_MouseOffsetX;
		Click.y -= *p_D2CLIENT_MouseOffsetY;
		
		Vars.bClickAction = TRUE;

		D2CLIENT_clickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : 0x08);
		D2CLIENT_SetSelectedUnit(NULL);

		Vars.bClickAction = FALSE;
		Vars.dwSelectedUnitId = NULL;
		Vars.dwSelectedUnitType = NULL;

		return TRUE;
	}

	Vars.dwSelectedUnitId = NULL;
	Vars.dwSelectedUnitType = NULL;

	POINT Click = {wX, wY};

	D2COMMON_MapToAbsScreen(&Click.x, &Click.y);

	Click.x -= *p_D2CLIENT_MouseOffsetX;
	Click.y -= *p_D2CLIENT_MouseOffsetY;

	Vars.bClickAction = TRUE;
	D2CLIENT_clickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : 8);
	Vars.bClickAction = FALSE;

	return TRUE;
}
