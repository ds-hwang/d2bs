#include "D2Utilities.h"
#include "Structs.h"
#include "Functions.h"
#include "Helpers.h"

bool GameReady(void)
{
	UnitAny* player = GetPlayerUnit();
	return player && player->pInventory && player->pPath &&
			player->pPath->xPos && player->pPath->pRoom1 &&
			player->pPath->pRoom1->pRoom2 &&
			player->pPath->pRoom1->pRoom2->pLevel &&
			player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo;
}

void D2Print(char * szText)
{
	wchar_t * szTextUnicode = AnsiToUnicode(szText);

	if (GameReady())
	{
		PrintGameString(szTextUnicode, 0);
	}
	else
	{
		MessageBox(0, szText, "D2BS", 0);
	}

	delete[] szTextUnicode;
}

UnitAny* FindUnit(DWORD id, DWORD type)
{
	UnitAny* unit = FindServerSideUnit(id, type);
	return unit ? unit : FindClientSideUnit(id, type);
}
