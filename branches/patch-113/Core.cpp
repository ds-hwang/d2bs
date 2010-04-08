#include <string>
#include <sstream>
#include <algorithm>
#include <list>

#include "Core.h"
#include "D2Ptrs.h"
//#include "D2Helpers.h"
#include "Helpers.h"
#include "Control.h"
#include "CriticalSections.h"
#include "Console.h"

bool SplitLines(const std::string & str, size_t maxlen, const char delim, std::list<std::string> & lst)
{
	using namespace std;

	if(str.length() < 1 || maxlen < 2)
		return false;

	size_t pos, len;
	string tmp(str);

	while(tmp.length() > maxlen)
	{
		len = tmp.length();
		// maxlen-1 since std::string::npos indexes from 0
		pos = tmp.find_last_of(delim, maxlen-1);
		if(!pos || pos == string::npos)
		{
			//Target delimiter was not found, breaking at maxlen
			// maxlen-1 since std::string::npos indexes from 0
			lst.push_back(tmp.substr(0, maxlen-1));
			tmp.erase(0, maxlen-1);
			continue;
		}
		pos = tmp.find_last_of(delim, maxlen-1);
		if(pos && pos != string::npos)
		{
			//We found the last delimiter before maxlen
			lst.push_back(tmp.substr(0, pos) + delim);
			tmp.erase(0, pos);
		}
		else
			DebugBreak();
	}
	if(!tmp.length())
		DebugBreak();

	if(tmp.length())
		lst.push_back(tmp);

	return true;
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
	if(Vars.bUseGamePrint)
	{
		if(ClientState() == ClientStateInGame)
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
	}
	for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
			Console::AddLine(*it);

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
	Vars.bDontCatchNextMsg = TRUE;

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

bool ClickMap(DWORD dwClickType, int wX, int wY, BOOL bShift, UnitAny* pUnit)
{
	if(ClientState() != ClientStateInGame)
		return false;

	POINT Click = {wX, wY};
	if(pUnit)
	{
		Click.x = GetUnitX(pUnit);
		Click.y = GetUnitY(pUnit);
	}

	D2COMMON_MapToAbsScreen(&Click.x, &Click.y);

	Click.x -= *p_D2CLIENT_MouseOffsetX;
	Click.y -= *p_D2CLIENT_MouseOffsetY;

	if(pUnit && pUnit != D2CLIENT_GetPlayerUnit())
	{
		Vars.dwSelectedUnitId = pUnit->dwUnitId;
		Vars.dwSelectedUnitType = pUnit->dwType;

		Vars.bClickAction = TRUE;

		D2CLIENT_ClickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : 0x08);
		D2CLIENT_SetSelectedUnit(NULL);

		Vars.bClickAction = FALSE;
		Vars.dwSelectedUnitId = NULL;
		Vars.dwSelectedUnitType = NULL;
	}
	else
	{
		Vars.dwSelectedUnitId = NULL;
		Vars.dwSelectedUnitType = NULL;

		Vars.bClickAction = TRUE;
		D2CLIENT_ClickMap(dwClickType, Click.x, Click.y, bShift ? 0x0C : 8);
		Vars.bClickAction = FALSE;
	}

	return TRUE;
}
