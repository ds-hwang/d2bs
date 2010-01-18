#include "D2NetHandlers.h"
#include "Script.h"
#include "ScreenHook.h"
#include "Unit.h"
#include "Helpers.h"
#include "Core.h"
#include "Constants.h"
#include "Events.h"
#include "CollisionMap.h"
#include "ScriptEngine.h"
#include "Console.h"
#include "D2BS.h"

using namespace std;

Variables Vars;

DWORD ReassignPlayerHandler(BYTE* pPacket, DWORD dwSize)
{
	if(*(LPDWORD)&pPacket[2] == D2CLIENT_GetPlayerUnit()->dwUnitId)
		pPacket[10] = NULL;

	return TRUE;
}

DWORD HPMPUpdateHandler(BYTE* pPacket, DWORD dwSize)
{
	WORD Life = *(WORD*)&pPacket[1];
	WORD Mana = *(WORD*)&pPacket[3];

	if((Life & 0x8000) == 0x8000)
	{
		Life ^= 0x8000;
	}
	if((Mana & 0x8000) == 0x8000)
	{
		Mana ^= 0x8000;
	}
	if((Mana & 0x4000) == 0x4000)
	{
		Mana ^= 0x4000;
	}
	Mana *= 2;

	static WORD SaveLife = 0;
	if(SaveLife != Life)
	{
		SaveLife = Life;
		LifeEvent(Life);
	}

	static WORD SaveMana = 0;
	if(SaveMana != Mana)
	{
		SaveMana = Mana;
		ManaEvent(Mana);
	}

	return TRUE;
}

DWORD ChatEventHandler(BYTE* pPacket, DWORD dwSize)
{
	CHAR* pName = (CHAR*)pPacket+10;
	CHAR* pMessage = (CHAR*)pPacket + strlen(pName) + 11;
	ChatEvent(pName, pMessage);

	return TRUE;
}

DWORD NPCTransactionHandler(BYTE* pPacket, DWORD dwSize)
{
	return TRUE;
}

DWORD EventMessagesHandler(BYTE* pPacket, DWORD dwSize)
{
	switch (pPacket[1])
	{
		case 0x07:
			{
				if( Vars.bQuitOnHostile && pPacket[2] == 0x08 )
				{
					D2CLIENT_ExitGame();
				}
			}
			break;

		case 0x11:
			{
				DWORD soj = *(DWORD*)&pPacket[3];
				char mess[256]; 
				sprintf_s(mess, sizeof(mess), "%u Stones of Jordan Sold to Merchants", soj);				
				GameMsgEvent(mess);
			}
			break;

		case 0x12:
			{
				char mess[] ="Diablo Walks the Earth";
				GameMsgEvent(mess);
			}
			break;
	}

	return TRUE;
}

DWORD ItemActionHandler(BYTE* pPacket, DWORD dwSize)
{
	INT64 icode   = 0;
	CHAR code[5]  = "";
	BYTE mode     = pPacket[1];
	DWORD gid     = *(DWORD*)&pPacket[4];
	BYTE dest     = ((pPacket[13] & 0x1C) >> 2);

	switch(dest)
	{
		case 0: case 2:
			icode = *(INT64 *)(pPacket+15)>>0x04;
			break;
		case 3: case 4: case 6:
			if(!((mode == 0 || mode == 2) && dest == 3))
			{
				if(mode != 0xF && mode != 1)
					icode = *(INT64 *)(pPacket+17) >> 0x1C;
				else
					icode = *(INT64 *)(pPacket+15) >> 0x04;
			} else  icode = *(INT64 *)(pPacket+17) >> 0x05;
			break;
		default:
			Log("Received invalid item destination...? mode = %d, gid = %d, dest = %d", mode, gid, dest);
			break;
	}

	// Converting and Cleaning
	memcpy(code, &icode, 4);
	if(code[3] == ' ') code[3] = '\0';

	if(strcmp(code, "gld") == 0)
		GoldDropEvent(gid, mode);
	else
		ItemActionEvent(gid, code, mode, (pPacket[0] == 0x9d));	

	return TRUE;
}

DWORD DelayedStateHandler(BYTE* pPacket, DWORD dwSize)
{
	if(pPacket[6] == AFFECT_JUST_PORTALED)
		return FALSE;

	return TRUE;
}
