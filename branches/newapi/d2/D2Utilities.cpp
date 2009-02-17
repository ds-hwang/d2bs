#include "D2Utilities.h"
#include "Structs.h"
#include "Functions.h"
#include "Helpers.h"

#include "SyncLock.h"

#include "Script.h"

#include "js32.h"

#include "debug_new.h"

void GamePrint(const char*);
void OOGPrint(const char*);
void StatusPrint(const char*);
char* PrintHelper(char*, va_list);


void D2BSCleanup()
{
	Log("D2BS Cleanup");
	Script::Shutdown();
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

void D2Print(char* szText, ...)
{
	va_list args;
	va_start(args, szText);
	char* text = PrintHelper(szText, args);
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

char* PrintHelper(char* szText, va_list args)
{
	// return NULL when the string has been fully formatted or something? we gotta figure that one out...
	int len = _vscprintf(szText, args);
	char* text = new char[len+1];
	vsprintf(text, szText, args);

	// TODO: Make this format and wrap messages and so forth

	return text;
}

void GamePrint(const char* text)
{
	//PrintLock lock;
	wchar_t* wtext = AnsiToUnicode(text);
	PrintGameString(wtext, 0);
	delete[] wtext;
	//delete lock;
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
