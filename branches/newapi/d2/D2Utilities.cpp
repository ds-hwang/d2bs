#include "D2Utilities.h"
#include "Structs.h"
#include "Functions.h"
#include "Helpers.h"

#include "SyncLock.h"

#include "Script.h"

#include "js32.h"

#include "debug_new.h"

extern PRThread* mainThread;
extern HHOOK hKeybHook, hMouseHook;
extern WNDPROC oldWndProc;

void GamePrint(const char* text);
void OOGPrint(const char* text);
void StatusPrint(const char* text);
char* PrintHelper(const char* format, va_list args);

void D2BSCleanup()
{
	Log("D2BS Cleanup");
	Script::Shutdown();
	PR_JoinThread(mainThread);
	UnhookWindowsHookEx(hKeybHook);
	UnhookWindowsHookEx(hMouseHook);
	SetWindowLong(GetHwnd(), GWL_WNDPROC, (LONG)oldWndProc);
}

bool GameReady(void)
{
	UnitAny* player = GetPlayerUnit();
	return player && player->pInventory && player->pPath &&
			player->pPath->xPos && player->pPath->pRoom1 &&
			player->pPath->pRoom1->pRoom2 &&
			player->pPath->pRoom1->pRoom2->pLevel &&
			player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
}

void D2Print(char* format, ...)
{
	va_list args;
	va_start(args, format);
	char* text = PrintHelper(format, args);
	va_end(args);

	if(GameReady())
	{
		GamePrint(text);
	}
	else if(false) // TODO: Make this detect chat vs. non-chat
	{
		OOGPrint(text);
	}
	else
	{
		StatusPrint(text);
	}

	// delete[] from PrintHelper
	delete[] text;
}

char* PrintHelper(char* format, va_list args)
{
	// return NULL when the string has been fully formatted or something? we gotta figure that one out...
	int len = _vscprintf(format, args);
	char* text = new char[len+1];
	vsprintf(text, format, args);

	// TODO: Make this format and wrap messages and so forth

	return text;
}

void GamePrint(const char* text)
{
	PrintLock lock;
	wchar_t* wtext = AnsiToUnicode(text);
	PrintGameString(wtext, 0);
	delete[] wtext;
}

void OOGPrint(const char* text)
{
	// TODO: is this multithreaded?
	PrintChannelText(text, 0);
}

void StatusPrint(const char* text)
{
	// TODO: make this print to our status console, not in a message box
	MessageBox(0, text, "D2BS", 0);
}

UnitAny* FindUnit(DWORD id, DWORD type)
{
	UnitAny* unit = FindServerSideUnit(id, type);
	return unit ? unit : FindClientSideUnit(id, type);
}

RosterUnit* FindPlayerRoster(DWORD id)
{
	for(RosterUnit* pRoster = p_D2CLIENT_PlayerUnitList; pRoster; pRoster = pRoster->pNext)
		if(pRoster->dwUnitId == id)
			return pRoster;
}

