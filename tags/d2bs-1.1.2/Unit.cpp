#include "D2Ptrs.h"
#include "Unit.h"
#include "Constants.h"
#include "D2Helpers.h"

#define HAS_BIT(value, bit) ((((value) >> (bit)) & 0x1) == 0x1)

UnitAny* GetUnit(char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId)
{
	if(!GameReady())
		return NULL;

	// First off, check for near units
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	if(player && player->pPath && player->pPath->pRoom1 && player->pPath->pRoom1->pRoom2 && 
		player->pPath->pRoom1->pRoom2->pLevel)
	{
		Room2* ptRoomOther = player->pPath->pRoom1->pRoom2->pLevel->pRoom2First;

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
	}
	return NULL;
}

UnitAny* GetNextUnit(UnitAny* pUnit, char* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode)
{
	if(!GameReady())
		return NULL;

	if(!pUnit)
		return NULL;

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

	return NULL;
}

UnitAny* GetInvUnit(UnitAny* pOwner, char* szName, DWORD dwClassId, DWORD dwMode, DWORD dwUnitId)
{
	for(UnitAny* pItem = D2COMMON_GetItemFromInventory(pOwner->pInventory); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
	{
		if(CheckUnit(pItem, szName, dwClassId, 4, dwMode, dwUnitId))
			return pItem;
	}

	return NULL;
}

UnitAny* GetInvNextUnit(UnitAny* pUnit, UnitAny* pOwner, char* szName, DWORD dwClassId, DWORD dwMode)
{
	if(pUnit->dwType == UNIT_ITEM)
	{
		// Check first if it belongs to a person
		if(pUnit->pItemData && pUnit->pItemData->pOwnerInventory && pUnit->pItemData->pOwnerInventory == pOwner->pInventory)
		{
			// Get the next matching unit from the owner's inventory
			for(UnitAny* pItem = D2COMMON_GetNextItemFromInventory(pUnit); pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
			{
				if(CheckUnit(pItem, szName, dwClassId, 4, dwMode, NULL))
					return pItem;
			}

		}
	}

	return NULL;
}

BOOL CheckUnit(UnitAny* pUnit, CHAR* szName, DWORD dwClassId, DWORD dwType, DWORD dwMode, DWORD dwUnitId)
{
	if(!GameReady())
		return FALSE;

	if(pUnit->dwType != dwType)
		return FALSE;

	if(dwMode != -1)
	{
		if(dwMode >= 100 && pUnit->dwType == UNIT_ITEM)
		{
			if(pUnit->pItemData && dwMode-100 != pUnit->pItemData->ItemLocation)
				return FALSE;
		}
		else
		{
			if(HAS_BIT(dwMode, 29))
			{
				bool result = false;
				// mode is a mask
				for(unsigned int i = 0; i < 28; i++)
					if(HAS_BIT(dwMode, i) && pUnit->dwMode == i)
						result = true;
				if(!result)
					return FALSE;
			}
			else if(pUnit->dwMode != dwMode)
				return FALSE;
		}
	}

	if(dwUnitId && dwUnitId == pUnit->dwUnitId)
		return TRUE;
	else if(dwUnitId)
		return FALSE;

	if(szName && szName[0])
	{
		char szBuf[512] = "";

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
			GetUnitName(pUnit, szBuf, 512);

		if(strstr(szBuf, szName))
			return TRUE;
	}
	else if(dwClassId != -1 && pUnit->dwTxtFileNo == dwClassId)
		return TRUE;

	if((!szName || !szName[0]) && dwClassId == -1)
		return TRUE;

	return FALSE;
}

INT GetUnitHP(UnitAny* pUnit)
{
	return (INT)(GetUnitStat(pUnit, STAT_HP) >> 8);
}

INT GetUnitMP(UnitAny* pUnit)
{
	return (INT)(GetUnitStat(pUnit, STAT_MANA) >> 8);
}
