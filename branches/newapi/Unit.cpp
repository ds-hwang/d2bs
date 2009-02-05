//#include "D2BS.h"
#include "D2Ptrs.h"
#include "Unit.h"
#include "Constants.h"
#include "D2Helpers.h"

#include "debugnew/debug_new.h"

UnitAny* GetUnit(CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId)
{
	// First off all, check for near units
	Room2* ptRoomOther = NULL;

	UnitAny* player = D2CLIENT_GetPlayerUnit();
	if(player && player->pPath && player->pPath->pRoom1 &&
		player->pPath->pRoom1->pRoom2 && player->pPath->pRoom1->pRoom2->pLevel)
			ptRoomOther = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First;

	for(;ptRoomOther; ptRoomOther = ptRoomOther->pRoom2Next)
	{
		if(!ptRoomOther->pRoom1)
			continue;
		for(UnitAny* lpUnit = ptRoomOther->pRoom1->pUnitFirst; lpUnit; lpUnit = lpUnit->pListNext)
		{
			if(CheckUnit(lpUnit, szName, dwClassId, dwType, dwMode, dwUnitId))
				return lpUnit;
		}
	}

	// No match, check the inventory of the unit we are currently interacted with
	UnitAny* npc = D2CLIENT_GetCurrentInteractingNPC();
	if(D2CLIENT_GetUIState(UI_NPCSHOP) && npc)
		for(UnitAny* pItem = D2COMMON_GetItemFromInventory(npc->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
		{
			if(CheckUnit(pItem, szName, dwClassId, dwType, dwMode, dwUnitId))
				return pItem;
		}

	// We couldn't find it, let's check our Inventory
	for(UnitAny* pItem = D2COMMON_GetItemFromInventory(player->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
	{
		if(CheckUnit(pItem, szName, dwClassId, dwType, dwMode, dwUnitId))
			return pItem;
	}

	return NULL;
}

UnitAny* GetNextUnit(UnitAny* pUnit, CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode)
{
	if(!pUnit)
		return NULL;

	if(pUnit->dwType == dwType && dwType == UNIT_ITEM) // Yep, it's an item. Check first if it belongs to a person..!
	{
		if(pUnit->pItemData && pUnit->pItemData->pOwnerInventory)
		{
			for(UnitAny* pItem = D2COMMON_GetNextItemFromInventory(pUnit); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
			{
				if(CheckUnit(pItem, szName, dwClassId, dwType, dwMode, NULL))
					return pItem;
			}

			return NULL;
		}
		// Was the item we looked for in an NPC inventory? If so .. Check our inventory!
		if((pUnit->pItemData && pUnit->pItemData->pOwnerInventory && pUnit->pItemData->pOwnerInventory->pOwner) &&
			(pUnit->pItemData->pOwnerInventory->pOwner->dwType != UNIT_PLAYER ||
				pUnit->pItemData->pOwnerInventory->pOwner->dwUnitId != D2CLIENT_GetPlayerUnit()->dwUnitId))
		{
			for(UnitAny* pItem = D2COMMON_GetItemFromInventory(D2CLIENT_GetPlayerUnit()->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
			{
				if(CheckUnit(pItem, szName, dwClassId, dwType, dwMode, NULL))
					return pItem;
			}				
		}
	}

	// OKAY, it's not located in an inventory, lets scan throught the rooms..
	
	UnitAny* lpUnit = pUnit->pListNext;
	Room1* ptRoom = D2COMMON_GetRoomFromUnit(pUnit);
	Room2* ptRoomOther = NULL;

	if(ptRoom)
	{
		ptRoomOther = ptRoom->pRoom2;

		if(!lpUnit && ptRoomOther)
			ptRoomOther = ptRoomOther->pRoom2Next;

		for(; ptRoomOther; ptRoomOther = ptRoomOther->pRoom2Next)
		{
			if(ptRoomOther->pRoom1)
			{
				if(!lpUnit)
					lpUnit = ptRoomOther->pRoom1->pUnitFirst;

				for(; lpUnit; lpUnit = lpUnit->pListNext)
				{
					if(CheckUnit(lpUnit, szName, dwClassId, dwType, dwMode, NULL))
						return lpUnit;
				}
			}
		}
	}

	// Item wasn't found on the ground -> check the inventory again ..!
	if(pUnit->dwType == UNIT_ITEM && (!pUnit->pItemData || !pUnit->pItemData->pOwnerInventory))
	{
		for(UnitAny* pItem = D2COMMON_GetItemFromInventory(D2CLIENT_GetPlayerUnit()->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
		{
			if(CheckUnit(pItem, szName, dwClassId, dwType, dwMode, NULL))
				return pItem;
		}
	}

	return NULL;
}

// Cuts down the code size :)
BOOL CheckUnit(UnitAny* pUnit, CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId)
{
	if(pUnit->dwType != dwType)
		return FALSE;

	if(dwMode != -1)
	{
		if(pUnit->dwType == UNIT_ITEM)
			if(dwMode >= 100)
			{
				if(pUnit->pItemData)
					if(dwMode-100 != pUnit->pItemData->ItemLocation)
						return FALSE;
			}
			else
				if(pUnit->dwMode != dwMode)
					return FALSE;
	}

	if(dwUnitId && dwUnitId == pUnit->dwUnitId)
		return TRUE;
	else if(dwUnitId)
		return FALSE;

	if(szName && szName[0])
	{
    	CHAR szBuf[8192] = "";

			if(dwType == UNIT_ITEM)
			{
				ItemTxt* pTxt = D2COMMON_GetItemText(pUnit->dwTxtFileNo);
				
				if(pTxt)
				{
					memcpy(szBuf, pTxt->szCode, 3);
					szBuf[3] = 0x00;
				}
			}
			else
				GetUnitName(pUnit, szBuf);

		if(strstr(szBuf, szName))
			return TRUE;
	}
	else if(dwClassId != -1)
	{
		if(pUnit->dwTxtFileNo == dwClassId)
			return TRUE;
	}

	if( (!szName || !szName[0]) && dwClassId == -1)
		return TRUE;

	return FALSE;
}

INT GetUnitHP(UnitAny* pUnit)
{
	return (INT)(GetUnitStat(D2CLIENT_GetPlayerUnit(), STAT_HP) >> 8);
}

INT GetUnitMP(UnitAny* pUnit)
{
	return (INT)(GetUnitStat(D2CLIENT_GetPlayerUnit(), STAT_MANA) >> 8);
}
