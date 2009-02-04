#include "Core.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"
#include "Control.h"
#include "CriticalSections.h"

#include <string>
#include <sstream>
#include <algorithm>
#include <list>

#include "debugnew/debug_new.h"


//! Splits a given string in two substrings, trying not to cut words.
//!
//! @param str The string to split.
//! @param point Point in which to split. Will split at the previous space if possible,
//! or at the point itself.
//! @param lst The list in which to insert.
//! @param split The iterator in which to insert in the list.
static void SplitStr(const std::string & str, size_t point, 
					 std::list<std::string> & lst, 
					 std::list<std::string>::iterator split)
{
	using namespace std;

	string::const_reverse_iterator it = 
		find( string::const_reverse_iterator(str.begin() + point + 1), str.rend(), ' ');

	// Split after the space or in the point itself if no spaces are found.
	string::const_iterator splitPoint(
		it != str.rend() ? it.base() : str.begin() + point
		);

	string temp;
	copy(str.begin(), splitPoint, back_inserter(temp));
	lst.insert(split, temp);
	temp.clear();
	copy(splitPoint, str.end(), back_inserter(temp));
	lst.insert(split, temp);
}


void Print(const char * szFormat, ...)
{
	using namespace std;

	const char REPLACE_CHAR = (char)0xFE;

	va_list vaArgs;
	va_start(vaArgs, szFormat);
	int len = _vscprintf(szFormat, vaArgs);
	char* str = new char[len+1];
	vsprintf_s(str, len+1, szFormat, vaArgs);
	va_end(vaArgs);

	replace(str, str + len, REPLACE_CHAR, '%');

	const size_t MAXLEN = 500;

	// Break into lines through \n.
	list<string> lines;
	string temp;

	stringstream ss(str);
	while(getline(ss, temp))
		lines.push_back(temp);

	// Make sure every line is smaller than MAXLEN.
	for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		// If one line is longer, we split it.
		int siz = it->size();
		if(it->size() > MAXLEN)
		{
			list<string>::iterator nxi(it);
			SplitStr(*it, MAXLEN, lines, ++nxi);

			// We erase the line that is too long.
			list<string>::iterator tempit = it;
			++it;
			lines.erase(tempit);
		}
	}

	EnterCriticalSection(&Vars.cPrintSection);
	if(D2CLIENT_GetPlayerUnit() && GameReady())
	{
		// Convert and send every line.
		for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
		{
			wchar_t * output = AnsiToUnicode(it->c_str());
			D2CLIENT_PrintGameString(output, 0);
			delete [] output;
		}
	}
	else if(findControl(4,28,410,354,298)) {
			// TODO: Double check this function, make sure it is working as intended.
			// D2MULTI_PrintChannelText(szString, NULL);
	} else {
			// Print original string.
			MessageBox(0, str, "D2BS " D2BS_VERSION, 0);
	}
	LeaveCriticalSection(&Vars.cPrintSection);

	delete [] str;
}



//VOID Print(const char * szFormat, ...)
//{
//	char * szNCFormat = (char*)szFormat;
//
//	// TODO: Come up with a better replacement for 0x255
//#define REPLACE_CHAR (char)0x255
//	char* c = 0;
//	while((c = strchr(szNCFormat, '%')) != 0)
//		*c = REPLACE_CHAR;
//
//	char* szString = NULL;
//	va_list vaArgs;
//
//	va_start(vaArgs, szNCFormat);
//	int len = _vscprintf(szNCFormat, vaArgs);
//	szString = new char[len+1];
//	vsprintf_s(szString, len+1, szNCFormat, vaArgs);
//	va_end(vaArgs);
//
//	c = 0;
//	while((c = strchr(szString, REPLACE_CHAR)) != 0)
//		*c = '%';
//#undef REPLACE_CHAR
//
//#define MAXLEN 500
//	if(len > MAXLEN)
//	{
//		for(int i = 0; i < (len/MAXLEN)+1; i++)
//		{
//			char tmp[MAXLEN+1] = "";
//			memcpy(tmp, szString+i*MAXLEN, MAXLEN);
//			Print(tmp);
//		}
//		delete[] szString;
//		return;
//	}
//#undef MAXLEN
//
//	EnterCriticalSection(&Vars.cPrintSection);
//	if(D2CLIENT_GetPlayerUnit() && GameReady())
//	{
//		wchar_t* wOutput = AnsiToUnicode(szString);
//		// the 200 character limit seems to have been lifted... longer than 200 characters didn't crash
//		D2CLIENT_PrintGameString(wOutput, 0);
//		delete[] wOutput;
//	} else if(findControl(4,28,410,354,298)) {
//		// TODO: Double check this function, make sure it is working as intended.
//		// D2MULTI_PrintChannelText(szString, NULL);
//	} else {
//		MessageBox(0, szString, "D2BS " D2BS_VERSION, 0);
//	}
//
//	LeaveCriticalSection(&Vars.cPrintSection);
//	delete[] szString;
//}



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

void Say(char *szMessage, ...) 
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

bool ClickMap(DWORD dwClickType, WORD wX, WORD wY, BOOL bShift, UnitAny* pUnit)
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
