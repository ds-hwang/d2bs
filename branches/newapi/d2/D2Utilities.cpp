#include "D2Utilities.h"
#include "Structs.h"
#include "Functions.h"
#include "Helpers.h"

#include "SyncLock.h"

#include "debug_new.h"

void GamePrint(const char*);
void OOGPrint(const char*);
void StatusPrint(const char*);
const char* PrintHelper(char*, va_list);

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
	const char* text = PrintHelper(szText, args);
	va_end(args);

	if(GameReady())
	{
		GamePrint(szText);
	}
	else if(false) // TODO: Make this detect chat vs. non-chat
	{
		OOGPrint(szText);
	}
	else
	{
		StatusPrint(szText);
	}

	// delete[] from PrintHelper
	delete[] text;
}

const char* PrintHelper(char* szText, va_list args)
{
	// TODO: Make this format and wrap messages and so forth
	// return NULL when the string has been fully formatted or something? we gotta figure that one out...
	char* text = NULL;

	int len = _vscprintf(szText, args);
	text = new char[len];
	vsprintf(text, szText, args);

	return text;
}

void GamePrint(const char* text)
{
	// TODO: surround this with a critical section for locking, because PrintGameString isn't multithreaded
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
