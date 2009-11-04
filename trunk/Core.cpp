#include <string>
#include <sstream>
#include <algorithm>
#include <list>

#include "Core.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"
#include "Control.h"
#include "CriticalSections.h"
#include "Console.h"

#include "debugnew/debug_new.h"

//! Splits a given string into lines trying not to cut words.
//!
//! @param str The string to split.
//! @param maxlen The maximum length of a split line.
//! @param delim Delimiter to attempt to split at
//! Will split at the delimiter if possible or at max length itself.
//! @param lst The list to insert the lines into.
static void SplitLines(const std::string & str, uint maxlen, const char delim, std::list<std::string> & lst)
{
	using namespace std;

	uint pos;
	string tmp = str;
	string chunk;

	if(tmp.length() < maxlen)
		lst.push_back(tmp);
	else
	{
		while(tmp.length() >= maxlen)
		{
			pos = tmp.find_last_of(delim, maxlen);
			if(pos >= maxlen)
				pos = maxlen;
			chunk = tmp.substr(0, pos);
			tmp.erase(0, pos);
			lst.push_back(chunk + " ");

			if(pos == string::npos)
				break;
		}
	}
}

void Print(const char * szFormat, ...)
{
	using namespace std;

	const char REPLACE_CHAR = (unsigned char)0xFE;

	va_list vaArgs;
	va_start(vaArgs, szFormat);
	int len = _vscprintf(szFormat, vaArgs);
	char* str = new char[len+1];
	vsprintf_s(str, len+1, szFormat, vaArgs);
	va_end(vaArgs);

	replace(str, str + len, REPLACE_CHAR, '%');

	const uint maxlen = 98;

	// Break into lines through \n.
	list<string> lines;
	string temp;
	stringstream ss(str);
	while(getline(ss, temp))
		SplitLines(temp, maxlen, ' ', lines);

	EnterCriticalSection(&Vars.cPrintSection);
#ifdef _DONT_USE_CONSOLE_
	if(GameReady())
	{
		// Convert and send every line.
		for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
		{
			wchar_t * output = AnsiToUnicode(it->c_str());
			D2CLIENT_PrintGameString(output, 0);
			delete [] output;
		}
	}
	else if(ClientState() == ClientStateMenu && findControl(4, (char *)NULL, -1, 28, 410, 354, 298)) 	
	{
		// TODO: Double check this function, make sure it is working as intended.
		for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
			D2MULTI_PrintChannelText((char* )it->c_str(), 0); 	
	}
	else
	{
		// Print original string.
		//MessageBox(0, str, "D2BS " D2BS_VERSION, 0);
		for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
			Console::AddLine(*it);
	}
#else
	for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
			Console::AddLine(*it);
#endif

	LeaveCriticalSection(&Vars.cPrintSection);

	delete[] str;
	str = NULL;
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

	if(*p_D2CLIENT_PlayerUnit)
	{
		wchar_t* wBuffer = AnsiToUnicode(szBuffer);
		memcpy((wchar_t*)p_D2CLIENT_ChatTextBuffer, wBuffer, wcslen(wBuffer)*2+1);
		delete[] wBuffer;
		wBuffer = NULL;

		MSG* aMsg = new MSG;
		aMsg->hwnd = D2WIN_GetHwnd();
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
	else
	{
		memcpy((CHAR*)p_D2MULTI_ChatBoxMsg, szBuffer, strlen(szBuffer) + 1);
		D2MULTI_DoChat();
	}
}

bool ClickMap(DWORD dwClickType, WORD wX, WORD wY, BOOL bShift, UnitAny* pUnit)
{
	if(!GameReady())
		return false;

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
