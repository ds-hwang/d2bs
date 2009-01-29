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

	if(len > 200)
	{
		for(int i = 0; i < (len/200)+1; i++)
		{
			char tmp[201] = "";
			memcpy(tmp, szString+i*200, 200);
			Print(tmp);
		}
		delete[] szString;
		return;
	}

	EnterCriticalSection(&Vars.cPrintSection);
	if(D2CLIENT_GetPlayerUnit() && GameReady())
	{
		wchar_t* wOutput = AnsiToUnicode(szString);
		if(wcslen(wOutput) > 200)
			D2CLIENT_PrintGameString(L"Failed to print: string longer than 200 chars!", 0);
		else
			D2CLIENT_PrintGameString(wOutput, 0);
		delete[] wOutput;
	} else if(*p_D2WIN_FirstControl) {
		D2MULTI_PrintChannelText(szString, NULL);
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
