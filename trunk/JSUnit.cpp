#include "JSUnit.h"
#include "CDebug.h"
#include "D2Ptrs.h"
#include "Constants.h"
#include "Helpers.h"
#include "Unit.h"
#include "Core.h"
#include "CriticalSections.h"
#include "D2Skills.h"

#include "debugnew/debug_new.h"

VOID unit_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("unit finalize");
	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(lpUnit)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete lpUnit;
	}
}

INT unit_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{	
	CDebug cDbg("unit getProperty");

	BnetData* pData = *p_D2LAUNCH_BnData;
	GameStructInfo* pInfo = *p_D2CLIENT_GameInfo;

	switch(JSVAL_TO_INT(id))
	{
		case ME_ACCOUNT:
			if(!pData)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pData->szAccountName));
			break;
		case ME_CHARNAME:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szCharName));
			break;
		case ME_CHICKENHP:
			*vp = INT_TO_JSVAL(Vars.nChickenHP);
			break;
		case ME_CHICKENMP:
			*vp = INT_TO_JSVAL(Vars.nChickenMP);
			break;
		case ME_DIFF:
			*vp = INT_TO_JSVAL(D2CLIENT_GetDifficulty());
			break;
		case ME_GAMENAME:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szGameName));
			break;
		case ME_GAMEPASSWORD:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szGamePassword));
			break;
		case ME_GAMESERVERIP:
			if(!pInfo)
				return JS_TRUE;
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pInfo->szGameServerIp));
			break;
		case ME_GAMESTARTTIME:
			*vp = INT_TO_JSVAL(Vars.dwGameTime);
			break;
		case ME_GAMETYPE:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_ExpCharFlag);
			break;
		case ME_ITEMONCURSOR:
			*vp = BOOLEAN_TO_JSVAL(!!D2CLIENT_GetCursorItem());
			break;
		case ME_LADDER:
			if(pData)
				*vp = BOOLEAN_TO_JSVAL(((pData->nCharFlags & PLAYER_TYPE_LADDER) == TRUE));
			break;
		case ME_QUITONHOSTILE:
			*vp = BOOLEAN_TO_JSVAL(Vars.bQuitOnHostile);
			break;
		case ME_REALM:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pData->szRealmName));
			break;
		case ME_REALMSHORT:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pData->szRealmName2));
			break;
		case OOG_SCREENSIZE:
			*vp = INT_TO_JSVAL(D2GFX_GetScreenSize());
			break;
		case OOG_WINDOWTITLE:
			CHAR szTitle[128];
			GetWindowText(D2WIN_GetHwnd(), szTitle, 128);
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szTitle));
			break;
		case ME_PING:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_Ping);
			break;
		case OOG_INGAME:
			*vp = BOOLEAN_TO_JSVAL(ClientState() == ClientStateMenu ? FALSE : TRUE);
			break;
		case OOG_QUITONERROR:
			*vp = BOOLEAN_TO_JSVAL(Vars.bQuitOnError);
			break;
		case OOG_MAXGAMETIME:
			*vp = INT_TO_JSVAL(Vars.dwMaxGameTime);
			break;
		case OOG_DEBUG:
			*vp = BOOLEAN_TO_JSVAL(Vars.bDebug);
			break;
		case ME_MERCREVIVECOST:
			*vp = INT_TO_JSVAL((*p_D2CLIENT_MercReviveCost));
			break;
		case ME_BLOCKKEYS:
			*vp = BOOLEAN_TO_JSVAL(Vars.bBlockKeys);
			break;
		case ME_BLOCKMOUSE:
			*vp = BOOLEAN_TO_JSVAL(Vars.bBlockMouse);
			break;
		default:
			break;
	}

/*
	// TODO: Properly fix this...
	if ((JSVAL_TO_INT(id) < OOG_WINDOWTITLE) && !GameReady())
			return JS_TRUE;
*/
	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	char* tmp = NULL;
	Room1* pRoom = NULL;

	switch(JSVAL_TO_INT(id))
	{
		case UNIT_TYPE:
			*vp = INT_TO_JSVAL(pUnit->dwType);
			break;
		case UNIT_CLASSID:
			*vp = INT_TO_JSVAL(pUnit->dwTxtFileNo);
			break;
		case UNIT_MODE:
			*vp = INT_TO_JSVAL(pUnit->dwMode);
			break;
		case UNIT_NAME:
			tmp = new char[8192];
			GetUnitName(pUnit, tmp, 8192);
			*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
			delete[] tmp;
			break;
		case UNIT_ACT:
			*vp = INT_TO_JSVAL(pUnit->dwAct + 1);
			break;
		case UNIT_AREA:
			pRoom = D2COMMON_GetRoomFromUnit(pUnit);
			if(pRoom && pRoom->pRoom2 && pRoom->pRoom2->pLevel)
				*vp = INT_TO_JSVAL(pRoom->pRoom2->pLevel->dwLevelNo);			
			break;
		case UNIT_ID:
			*vp = INT_TO_JSVAL(pUnit->dwUnitId);
			break;
		case UNIT_XPOS:
			*vp = INT_TO_JSVAL(GetUnitX(pUnit));
			break;
		case UNIT_YPOS:
			*vp = INT_TO_JSVAL(GetUnitY(pUnit));
			break;
		case UNIT_HP:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 6, 0) >> 8);
			break;
		case UNIT_HPMAX:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 7, 0) >> 8);
			break;
		case UNIT_MP:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 8, 0) >> 8);
			break;
		case UNIT_MPMAX:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 9, 0) >> 8);
			break;
		case UNIT_STAMINA:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 10, 0) >> 8);
			break;
		case UNIT_STAMINAMAX:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 11, 0) >> 8);
			break;
		case UNIT_CHARLVL:
			*vp = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, 12, 0));
			break;
		case ME_RUNWALK:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_AlwaysRun);
			break;
		case UNIT_ADDRESS:
			*vp = INT_TO_JSVAL(pUnit);
			break;
		case UNIT_SPECTYPE:
			DWORD SpecType;
			SpecType = NULL;
			if(pUnit->dwType == UNIT_MONSTER && pUnit->pMonsterData)
			{
				if(pUnit->pMonsterData->fMinion & 1)
					SpecType |= 0x08;
				if(pUnit->pMonsterData->fBoss & 1)
					SpecType |= 0x04;
				if(pUnit->pMonsterData->fChamp & 1)
					SpecType |= 0x02;
				if((pUnit->pMonsterData->fBoss & 1) && (pUnit->pMonsterData->fNormal & 1))
					SpecType |= 0x01;
				if(pUnit->pMonsterData->fNormal & 1)
					SpecType |= 0x00;
				*vp = INT_TO_JSVAL(SpecType);
				return JS_TRUE;
			}
			break;
		case UNIT_UNIQUEID:
			if(pUnit->dwType == UNIT_MONSTER && pUnit->pMonsterData->fBoss && pUnit->pMonsterData->fNormal)
				*vp = INT_TO_JSVAL(pUnit->pMonsterData->wUniqueNo);
			else
				*vp = INT_TO_JSVAL(-1);
			break;
		case ITEM_CODE: // replace with better method if found
			if(!(pUnit->dwType == UNIT_ITEM) && pUnit->pItemData)
				break;
			ItemTxt* pTxt;
			pTxt = D2COMMON_GetItemText(pUnit->dwTxtFileNo);
			if(!pTxt) {
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "Unknown"));
				return JS_TRUE;
			}
			CHAR szCode[4];
			memcpy(szCode, pTxt->szCode, 3);
			szCode[3] = 0x00;
			*vp = STRING_TO_JSVAL(JS_InternString(cx, szCode));
			break;
		case ITEM_PREFIX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				if (D2COMMON_GetItemMagicalMods(pUnit->pItemData->wPrefix))
					*vp = STRING_TO_JSVAL(JS_InternString(cx, D2COMMON_GetItemMagicalMods(pUnit->pItemData->wPrefix)));
			break;
			
		case ITEM_SUFFIX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				if (D2COMMON_GetItemMagicalMods(pUnit->pItemData->wSuffix))
					*vp = STRING_TO_JSVAL(JS_InternString(cx, D2COMMON_GetItemMagicalMods(pUnit->pItemData->wSuffix)));
			break;
		case ITEM_PREFIXNUM:
				if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
					*vp = INT_TO_JSVAL(pUnit->pItemData->wPrefix);
			break;
		case ITEM_SUFFIXNUM:
				if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
					*vp = INT_TO_JSVAL(pUnit->pItemData->wSuffix);
			break;
		case ITEM_FNAME:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				wchar_t wszfname[256] = L"";
				D2CLIENT_GetItemName(pUnit, wszfname, sizeof(wszfname));
				if(wszfname) {
					char* tmp = UnicodeToAnsi(wszfname);
					*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
					delete[] tmp;
				}
			}
			break;
		case ITEM_QUALITY:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->dwQuality);
			break;
		case ITEM_NODE:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->NodePage);
			break;
		case ITEM_LOC:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->ItemLocation);
			break;
		case ITEM_SIZEX:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				if(!D2COMMON_GetItemText(pUnit->dwTxtFileNo))
					break;
				*vp = INT_TO_JSVAL(D2COMMON_GetItemText(pUnit->dwTxtFileNo)->xSize);
			}
			break;
		case ITEM_SIZEY:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				if(!D2COMMON_GetItemText(pUnit->dwTxtFileNo))
					break;
				*vp = INT_TO_JSVAL(D2COMMON_GetItemText(pUnit->dwTxtFileNo)->ySize);
			}
			break;
		case ITEM_Type:
			if(pUnit->dwType == UNIT_ITEM && pUnit->pItemData) {
				if(!D2COMMON_GetItemText(pUnit->dwTxtFileNo))
					break;
				*vp = INT_TO_JSVAL(D2COMMON_GetItemText(pUnit->dwTxtFileNo)->nType);
			}
			break;
		case ITEM_DESC:
			{
			if(pUnit->dwType != UNIT_ITEM)
				break;

				wchar_t wBuffer[8192] = L"";
				D2CLIENT_GetItemDesc(pUnit, wBuffer);
				tmp = UnicodeToAnsi(wBuffer);
				*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
				delete[] tmp;
			}
			break;
		case UNIT_ITEMCOUNT:
			if(pUnit->pInventory)
				*vp = INT_TO_JSVAL(pUnit->pInventory->dwItemCount);
			break;
		case ITEM_BODYLOCATION:
			if(pUnit->dwType != UNIT_ITEM)
				break;
			if(pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->BodyLocation);
			break;
		case UNIT_OWNER:
			*vp = INT_TO_JSVAL(pUnit->dwOwnerId);
			break;
		case UNIT_OWNERTYPE:
			*vp = INT_TO_JSVAL(pUnit->dwOwnerType);
			break;
		case ITEM_LEVEL:
			if(pUnit->dwType != UNIT_ITEM)
				break;
			if(pUnit->pItemData)
				*vp = INT_TO_JSVAL(pUnit->pItemData->dwItemLevel);
			break;
		case ITEM_LEVELREQ:
			if(pUnit->dwType != UNIT_ITEM)
				break;
			*vp = INT_TO_JSVAL(D2COMMON_GetItemLevelRequirement(pUnit, D2CLIENT_GetPlayerUnit()));
			break;
		case UNIT_DIRECTION:
			if(pUnit->pPath)
				*vp = INT_TO_JSVAL(pUnit->pPath->bDirection);
			break;
		case OBJECT_TYPE:
			if(pUnit->dwType == UNIT_OBJECT)
				if(pUnit->pObjectData)
				{
					pRoom = D2COMMON_GetRoomFromUnit(pUnit);
					if(pRoom && pRoom->pRoom2 && pRoom->pRoom2->pLevel && IsTownLevel(pRoom->pRoom2->pLevel->dwLevelNo))
						*vp = INT_TO_JSVAL(pUnit->pObjectData->Type & 255);
					else
						*vp = INT_TO_JSVAL(pUnit->pObjectData->Type);
				}
			break;
		case ME_WSWITCH:
			*vp = INT_TO_JSVAL(*p_D2CLIENT_bWeapSwitch);
			break;
		default:
			break;
	}

	return JS_TRUE;
}

INT unit_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("unit setProperty");

	switch(JSVAL_TO_INT(id))
	{
		case ME_CHICKENHP:
			if(JSVAL_IS_INT(*vp))
				Vars.nChickenHP		= JSVAL_TO_INT(*vp);
			break;
		case ME_CHICKENMP:
			if(JSVAL_IS_INT(*vp))
				Vars.nChickenMP		= JSVAL_TO_INT(*vp);
			break;
		case ME_QUITONHOSTILE:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bQuitOnHostile = JSVAL_TO_BOOLEAN(*vp);
			break;
		case OOG_QUITONERROR:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bQuitOnError	= JSVAL_TO_BOOLEAN(*vp);
			break;
		case OOG_MAXGAMETIME:
			if(JSVAL_IS_INT(*vp))
				Vars.dwMaxGameTime	= JSVAL_TO_INT(*vp);
			break;
		case ME_BLOCKKEYS:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bBlockKeys = JSVAL_TO_BOOLEAN(*vp);
			break;
		case ME_BLOCKMOUSE:
			if(JSVAL_IS_BOOLEAN(*vp))
				Vars.bBlockMouse = JSVAL_TO_BOOLEAN(*vp);
			break;
	}
	return JS_TRUE;
}

INT unit_getUnit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getUnit");

	if(argc < 1)
		return JS_TRUE;

	jsint nType = NULL;
	jsint nClassId = -1;
	jsint nMode = -1;
	jsint nUnitId = NULL;
	CHAR szName[128] = "";

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nType = JSVAL_TO_INT(argv[0]);
	else
		return JS_TRUE;

	if(argc > 1 && JSVAL_IS_STRING(argv[1]))
		strcpy_s(szName, sizeof(szName), JS_GetStringBytes(JS_ValueToString(cx, argv[1])));
	
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		nClassId = JSVAL_TO_INT(argv[1]);

	if(argc > 2 && JSVAL_IS_INT(argv[2]))
		nMode = JSVAL_TO_INT(argv[2]);

	if(argc > 3 && JSVAL_IS_INT(argv[3]))
		nUnitId = JSVAL_TO_INT(argv[3]);

	UnitAny* pUnit = NULL;
	
	if(nType == 100)
		pUnit = D2CLIENT_GetCursorItem();
	else if(nType == 101)
	{
		pUnit = D2CLIENT_GetSelectedUnit();
		if(!pUnit)
			pUnit = (*p_D2CLIENT_SelectedInvItem);
	}
	else 
		pUnit = GetUnit(szName, nClassId, nType, nMode, nUnitId);

	if(!pUnit)
		return JS_TRUE;

	myUnit* pmyUnit = new myUnit; // leaked?

	if(!pmyUnit)
		return JS_TRUE;

	pmyUnit->_dwPrivateType = PRIVATE_UNIT;
	pmyUnit->dwClassId = nClassId;
	pmyUnit->dwMode = nMode;
	pmyUnit->dwType = pUnit->dwType;
	pmyUnit->dwUnitId = pUnit->dwUnitId;
	strcpy_s(pmyUnit->szName, sizeof(pmyUnit->szName), szName);

	JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);

	if(!jsunit)
		return JS_TRUE;

	*rval = OBJECT_TO_JSVAL(jsunit);

	return JS_TRUE;
}

INT unit_getNext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit getNext");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
		strcpy_s(lpUnit->szName, 128, JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		lpUnit->dwClassId = JSVAL_TO_INT(argv[0]);

	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		lpUnit->dwMode = JSVAL_TO_INT(argv[1]);

	pUnit = GetNextUnit(pUnit, lpUnit->szName, lpUnit->dwClassId, lpUnit->dwType, lpUnit->dwMode);

	if(!pUnit)
	{
		JS_ClearScope(cx, obj);
		if(JS_ValueToObject(cx, JSVAL_NULL, &obj) == JS_FALSE)
			return JS_TRUE;
		*rval = INT_TO_JSVAL(0);
	}
	else
	{
		lpUnit->dwUnitId = pUnit->dwUnitId;
		JS_SetPrivate(cx, obj, lpUnit);
		*rval = INT_TO_JSVAL(1);
	}

	return JS_TRUE;
}

INT unit_cancel(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit cancel");

	if(!GameReady())
		return JS_TRUE;

	if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		jsint nType = JSVAL_TO_INT(argv[0]);

		if(nType == 0)
			D2CLIENT_CloseInteract();
		else if(nType == 1)
			D2CLIENT_CloseNPCInteract();
	}
	else if(D2CLIENT_GetCursorItem())
	{
		// Diablo drops an Item by using the Walk function.
		// Just perform a clickMap "click" and we drop it
		D2CLIENT_clickMap(0, 10, 10, 0x08);
	}

	return JS_TRUE;
}

INT unit_repair(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit repair");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	BYTE aPacket[17] = { NULL };
	aPacket[0] = 0x35;
	*(DWORD*)&aPacket[1] = *p_D2CLIENT_RecentInteractId;
	aPacket[16] = 0x80;
	D2NET_SendPacket(17,1, aPacket);

	// note: this crashes while minimized
//	D2CLIENT_PerformNpcAction(pUnit,1, NULL);
	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

INT unit_useMenu(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit useMenu");

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	*rval = BOOLEAN_TO_JSVAL(ClickNPCMenu(pUnit->dwTxtFileNo, JSVAL_TO_INT(argv[0])));

	return JS_TRUE;
}

INT unit_interact(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit interact");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	*rval = JSVAL_FALSE;

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(pUnit->dwType == UNIT_ITEM && pUnit->dwMode != ITEM_MODE_ON_GROUND && pUnit->dwMode != ITEM_MODE_BEING_DROPPED)
	{
			INT nLocation = GetItemLocation(pUnit);					
			
			BYTE aPacket[13] = {NULL};

			if(nLocation == STORAGE_INVENTORY)
			{
				aPacket[0] = 0x20;
				*(DWORD*)&aPacket[1] = pUnit->dwUnitId;
				*(DWORD*)&aPacket[5] = D2CLIENT_GetPlayerUnit()->pPath->xPos;
				*(DWORD*)&aPacket[9] = D2CLIENT_GetPlayerUnit()->pPath->yPos;
				D2NET_SendPacket(13, 1, aPacket);
				return JS_TRUE;
			}
			else if(nLocation == STORAGE_BELT)
			{
				aPacket[0] = 0x26;
				*(DWORD*)&aPacket[1] = pUnit->dwUnitId;
				*(DWORD*)&aPacket[5] = 0;
				*(DWORD*)&aPacket[9] = 0;
				D2NET_SendPacket(13, 1, aPacket);
				return JS_TRUE;
			}
	}

	if(pUnit->dwType == UNIT_OBJECT && argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		// TODO: check the range on argv[0] to make sure it won't crash the game
		D2CLIENT_TakeWaypoint(pUnit->dwUnitId, JSVAL_TO_INT(argv[0]));
	//	D2CLIENT_TakeWP(pUnit->dwUnitId, JSVAL_TO_INT(argv[0]));
		
		*rval = JSVAL_TRUE;

		return JS_TRUE;
	}
	else if(pUnit->dwType == UNIT_PLAYER && argc == 1 && JSVAL_IS_INT(argv[0]) && JSVAL_TO_INT(argv[0]) == 1)
	{
		// Accept Trade
	}
	else
	{
		*rval = JSVAL_TRUE;
		ClickMap(0, GetUnitX(pUnit), GetUnitY(pUnit), FALSE, pUnit);
		//D2CLIENT_Interact(pUnit, 0x45);
	}

	return JS_TRUE;
}

INT unit_getStat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getStat");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	*rval = JSVAL_FALSE;

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	jsint nStat = JSVAL_TO_INT(argv[0]);
	jsint nSubIndex = NULL;

	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		nSubIndex = JSVAL_TO_INT(argv[1]);
	
	if(nStat >= 6 && nStat <= 11)
		*rval = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex)>>8);
	else if(nStat == 13)
	{
		JS_NewNumberValue(cx, D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex), rval);
		// testing new number code to allieviate this bad hack
/*		CHAR szExp[32] = "";
		sprintf(szExp, "%u", D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex));
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szExp)); */
	}
	else if(nStat == 92)
		*rval = INT_TO_JSVAL(D2COMMON_GetItemLevelRequirement(pUnit, D2CLIENT_GetPlayerUnit()));
	else if(nStat == -1)
	{
		Stat aStatList[256] = { NULL };
		StatList* pStatList = D2COMMON_GetStatList(pUnit, NULL, 0x40);

		if(pStatList)
		{
			DWORD dwStats = D2COMMON_CopyStatList(pStatList, (Stat*)aStatList, 256);

			JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

			if(pReturnArray)
			{
				for(UINT i = 0; i < dwStats; i++)
				{
					JSObject* pArrayInsert = JS_NewArrayObject(cx, 0, NULL);

					if(!pArrayInsert)
						continue;

					jsval nIndex	= INT_TO_JSVAL(aStatList[i].wStatIndex);
					jsval nSubIndex = INT_TO_JSVAL(aStatList[i].wSubIndex);
					jsval nValue	= INT_TO_JSVAL(aStatList[i].dwStatValue);

					JS_SetElement(cx, pArrayInsert, 0, &nIndex);
					JS_SetElement(cx, pArrayInsert, 1, &nSubIndex);	
					JS_SetElement(cx, pArrayInsert, 2, &nValue);	

					jsval aObj = OBJECT_TO_JSVAL(pArrayInsert);

					JS_SetElement(cx, pReturnArray, i, &aObj);
				}

				*rval = OBJECT_TO_JSVAL(pReturnArray);
			}
		}

	}
	else 
		*rval = INT_TO_JSVAL(D2COMMON_GetUnitStat(pUnit, nStat, nSubIndex));

	return JS_TRUE;
}

INT unit_getState(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getState");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	*rval = JSVAL_FALSE;

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	jsint nState;

	if(JS_ValueToInt32(cx, argv[0], &nState) == JS_FALSE)
		return JS_TRUE;

	// TODO: make these constants so we know what we're checking here
	if(nState > 159 || nState < 0)
		return JS_TRUE;

	*rval = BOOLEAN_TO_JSVAL(!!D2COMMON_GetUnitState(pUnit, nState));

	return JS_TRUE;
}

INT item_getFlags(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getFlags");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit->dwType == UNIT_ITEM)
		return JS_TRUE;

	*rval = INT_TO_JSVAL(pUnit->pItemData->dwFlags);

	return JS_TRUE;
}

INT item_getFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getFlag");

	if(!GameReady())
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_ITEM)
		return JS_TRUE;

	jsint nFlag = JSVAL_TO_INT(argv[0]);

	*rval = BOOLEAN_TO_JSVAL((nFlag & pUnit->pItemData->dwFlags));

	return JS_TRUE;
}

INT item_getPrice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getPrice");

	if(!GameReady())
		return JS_TRUE;

	INT diff = D2CLIENT_GetDifficulty();
	//D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pUnit, diff, *p_D2CLIENT_ItemPriceList, NPCID, buysell)
	INT buysell = 0;
	INT NPCID = 148;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(argc>0)
	{
		if(JSVAL_IS_OBJECT(argv[0]))
		{
			myUnit* pmyNpc = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
			
			if(!pmyNpc || IsBadReadPtr(pmyNpc, sizeof(myUnit)) || pmyNpc->_dwPrivateType != PRIVATE_UNIT)
				return JS_TRUE;

			UnitAny* pNpc = D2CLIENT_FindUnit(pmyNpc->dwUnitId, pmyNpc->dwType);

			if(!pNpc)
				return JS_TRUE;

			NPCID = pNpc->dwTxtFileNo;
		}
		else if(JSVAL_IS_INT(argv[0]))
			NPCID = JSVAL_TO_INT(argv[0]);
	}
	if(argc>1)
		buysell = JSVAL_TO_INT(argv[1]);
	if(argc>2)
		diff = JSVAL_TO_INT(argv[2]);

	*rval = INT_TO_JSVAL(D2COMMON_GetItemPrice(D2CLIENT_GetPlayerUnit(), pUnit, diff, *p_D2CLIENT_ItemPriceList, NPCID, buysell));

	return JS_TRUE;
}

INT unit_getItems(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getItems");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || !pUnit->pInventory || !pUnit->pInventory->pFirstItem)
		return JS_TRUE;

	JSObject* pReturnArray = JS_NewArrayObject(cx, NULL, NULL);
	
	if(!pReturnArray)
		return JS_TRUE;

	DWORD dwArrayCount = NULL;

	for(UnitAny* pItem = pUnit->pInventory->pFirstItem; pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
	{

		myUnit* pmyUnit = new myUnit;
		
		if(!pmyUnit)
			continue;

		pmyUnit->_dwPrivateType = PRIVATE_UNIT;
		pmyUnit->szName[0] = NULL;
		pmyUnit->dwMode = pItem->dwMode;
		pmyUnit->dwClassId = pItem->dwTxtFileNo;
		pmyUnit->dwUnitId = pItem->dwUnitId;
		pmyUnit->dwType = UNIT_ITEM;

		JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
		if (!jsunit)
			return JS_TRUE;
		jsval a = OBJECT_TO_JSVAL(jsunit);
		JS_SetElement(cx, pReturnArray, dwArrayCount, &a);
		dwArrayCount++;		
	}
	
	*rval = OBJECT_TO_JSVAL(pReturnArray);

	return JS_TRUE;
}

INT unit_getSkill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit getSkill");

	if(!GameReady())
		return JS_TRUE;

	jsint nSkillId = NULL;
	jsint nExt = NULL;

	if(argc == NULL)
		return JS_TRUE;

	if(argc == 1)
	{
		if(!JSVAL_IS_INT(argv[0]))
			return JS_TRUE;

		nSkillId = JSVAL_TO_INT(argv[0]);
	}
	else if(argc == 2)
	{
		if(!JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1]))
			return JS_TRUE;

		nSkillId = JSVAL_TO_INT(argv[0]);
		nExt = JSVAL_TO_INT(argv[1]);
	}
	if(argc == 1)
	{
		WORD wLeftSkillId = D2CLIENT_GetPlayerUnit()->pInfo->pLeftSkill->pSkillInfo->wSkillId;
		WORD wRightSkillId = D2CLIENT_GetPlayerUnit()->pInfo->pRightSkill->pSkillInfo->wSkillId;
		JSObject* pReturnArray = NULL;
		switch(nSkillId)
		{
			case 0: *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, Game_Skills[wRightSkillId].name)); break;
			case 1: *rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, Game_Skills[wLeftSkillId].name)); break;
			case 2: *rval = INT_TO_JSVAL(wRightSkillId); break;
			case 3: *rval = INT_TO_JSVAL(wLeftSkillId); break;
			case 4:
				pReturnArray = JS_NewArrayObject(cx, 0, NULL);
				if(pReturnArray)
				{
					int i = 0;
					for(Skill* pSkill = D2CLIENT_GetPlayerUnit()->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill) {
						JSObject* pArrayInsert = JS_NewArrayObject(cx, 0, NULL);

						if(!pArrayInsert)
							continue;

						jsval nId	= INT_TO_JSVAL(pSkill->pSkillInfo->wSkillId);
						jsval nBase = INT_TO_JSVAL(pSkill->dwSkillLevel);
						jsval nTotal = INT_TO_JSVAL(D2COMMON_GetSkillLevel(D2CLIENT_GetPlayerUnit(), pSkill, 1));

						JS_SetElement(cx, pArrayInsert, 0, &nId);
						JS_SetElement(cx, pArrayInsert, 1, &nBase);
						JS_SetElement(cx, pArrayInsert, 2, &nTotal);

						jsval aObj = OBJECT_TO_JSVAL(pArrayInsert);

						JS_SetElement(cx, pReturnArray, i, &aObj);
						i++;
					}
					*rval = OBJECT_TO_JSVAL(pReturnArray);
				}
				break;
			default:
				*rval = JSVAL_FALSE;
				break;
		}
		return JS_TRUE;
	}
	else if(argc == 2)
	{
		if(D2CLIENT_GetPlayerUnit() &&
			D2CLIENT_GetPlayerUnit()->pInfo &&
			D2CLIENT_GetPlayerUnit()->pInfo->pFirstSkill)
		{
			for(Skill* pSkill = D2CLIENT_GetPlayerUnit()->pInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
			{
				if(pSkill->pSkillInfo && pSkill->pSkillInfo->wSkillId == nSkillId)
				{
					*rval = INT_TO_JSVAL(D2COMMON_GetSkillLevel(D2CLIENT_GetPlayerUnit(), pSkill, nExt));
					return JS_TRUE;
				}
			}
		}

	}

	*rval = INT_TO_JSVAL(NULL);

	return JS_TRUE;
}

INT item_shop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("item shop");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpItem = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpItem || IsBadReadPtr(lpItem, sizeof(myUnit)) || lpItem->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pItem = D2CLIENT_FindUnit(lpItem->dwUnitId, lpItem->dwType);

	if(!pItem || pItem->dwType != UNIT_ITEM)
		return JS_TRUE;

	if(!D2CLIENT_GetUIState(UI_NPCSHOP))
		return JS_TRUE;

	CriticalMisc myMisc;
	myMisc.EnterSection();

	UnitAny* pNPC = D2CLIENT_GetCurrentInteractingNPC();
	DWORD dwMode = JSVAL_TO_INT(argv[argc - 1]);

	//Check if we are interacted.
	if (IsBadReadPtr(pNPC, sizeof(UnitAny)) || !pNPC)
		return JS_TRUE;

	//Check for proper mode.
	//if ((dwMode != 1) && (dwMode != 2) && (dwMode != 6))
	//	return JS_TRUE;

	//Selling an Item 
	if (dwMode == 1)
	{
		//Check if we own the item!
		if (pItem->pItemData->pOwnerInventory->pOwner->dwUnitId != (*p_D2CLIENT_PlayerUnit)->dwUnitId)
			return JS_TRUE;
		D2CLIENT_ShopAction(pItem, pNPC, pNPC, 1, (DWORD)0, 1, 1, NULL);
	}
	else
	{
		//Make sure the item is owned by the NPC interacted with.
		if (pItem->pItemData->pOwnerInventory->pOwner->dwUnitId != pNPC->dwUnitId)
			return JS_TRUE;

		D2CLIENT_ShopAction(pItem, pNPC, pNPC, 0, (DWORD)0, dwMode, 1, NULL);
	}

	//FUNCPTR(D2CLIENT, ShopAction, VOID __fastcall, (UnitAny* pItem, UnitAny* pNpc, UnitAny* pNpc2, DWORD dwSell, DWORD dwItemCost, DWORD dwMode, DWORD _2, DWORD _3), 0x19E00) // Updated

	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

INT unit_getParent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getParent");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(pUnit->dwType == UNIT_MONSTER)
	{
		DWORD dwOwnerId = D2CLIENT_GetMonsterOwner(pUnit->dwUnitId);
		UnitAny* pMonster = GetUnit(NULL, (DWORD)-1, UNIT_PLAYER, (DWORD)-1, dwOwnerId);
		if (!pMonster)
			pMonster = GetUnit(NULL, (DWORD)-1, UNIT_MONSTER, (DWORD)-1, dwOwnerId);
		if (!pMonster)
			return JS_TRUE;

		myUnit* pmyUnit = new myUnit;
			if(!pmyUnit)
				return JS_TRUE;	

		pmyUnit->_dwPrivateType = PRIVATE_UNIT;
		pmyUnit->dwUnitId = pMonster->dwUnitId;
		pmyUnit->dwClassId = pMonster->dwTxtFileNo;
		pmyUnit->dwMode = pMonster->dwMode;
		pmyUnit->dwType = pMonster->dwType;
		pmyUnit->szName[0] = NULL;
						
		JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
			if (!jsunit)
				return JS_TRUE;
		*rval = OBJECT_TO_JSVAL(jsunit);			
			return JS_TRUE;
	}
	else if(pUnit->dwType == UNIT_OBJECT)
	{
		if(pUnit->pObjectData)
		{
			CHAR szBuffer[128] = "";
			strcpy_s(szBuffer, sizeof(szBuffer), pUnit->pObjectData->szOwner);

			*rval = STRING_TO_JSVAL(JS_InternString(cx, szBuffer));
		}
	}
	else if(pUnit->dwType == UNIT_ITEM)
	{
		if(pUnit->pItemData && pUnit->pItemData->pOwnerInventory && pUnit->pItemData->pOwnerInventory->pOwner)
		{
			myUnit* pmyUnit = new myUnit;

			if(!pmyUnit)
				return JS_TRUE;

			pmyUnit->_dwPrivateType = PRIVATE_UNIT;
			pmyUnit->dwUnitId = pUnit->pItemData->pOwnerInventory->pOwner->dwUnitId;
			pmyUnit->dwClassId = pUnit->pItemData->pOwnerInventory->pOwner->dwTxtFileNo;
			pmyUnit->dwMode = pUnit->pItemData->pOwnerInventory->pOwner->dwMode;
			pmyUnit->dwType = pUnit->pItemData->pOwnerInventory->pOwner->dwType;
			pmyUnit->szName[0] = NULL;
			JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);

			*rval = OBJECT_TO_JSVAL(jsunit);
		}
	}

	return JS_TRUE;
}

// Works only on players sinces monsters _CANT_ have mercs!
INT unit_getMerc(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getMerc");

	if(!GameReady())
		return JS_TRUE;

	myUnit* lpUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!lpUnit || IsBadReadPtr(lpUnit, sizeof(myUnit)) || lpUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(lpUnit->dwUnitId, lpUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_PLAYER)
		return JS_TRUE;

	if(argc > 0 && JSVAL_IS_INT(argv[0]) && JSVAL_TO_INT(argv[0]) == 1)
	{
		UnitAny* pMerc = NULL;
		if(D2CLIENT_GetPlayerUnit()->pAct)
		{
			for(Room1* pRoom = D2CLIENT_GetPlayerUnit()->pAct->pRoom1; pRoom; pRoom = pRoom->pRoomNext)
			{
				for(UnitAny* pUnit = pRoom->pUnitFirst; pUnit; pUnit = pUnit->pListNext)
				{
					if(pUnit->dwType != UNIT_MONSTER)
						continue;

					if(pUnit->dwTxtFileNo == MERC_A1 || pUnit->dwTxtFileNo == MERC_A2 || pUnit->dwTxtFileNo == MERC_A3 || pUnit->dwTxtFileNo == MERC_A5)
					{
						if(D2CLIENT_GetMonsterOwner(pUnit->dwUnitId) == D2CLIENT_GetPlayerUnit()->dwUnitId)
						{
							*rval = JSVAL_TRUE;
							return JS_TRUE;
						}
					}
				}
			}
		}

		if(pMerc)
			*rval = JSVAL_TRUE;
		else if(*p_D2CLIENT_MercStrIndex == 0xFFFF)
			*rval = JSVAL_FALSE;
		else
			*rval = JSVAL_NULL;

		return JS_TRUE;
	}

	if(D2CLIENT_GetPlayerUnit() && D2CLIENT_GetPlayerUnit()->pAct)
	{
		for(Room1* pRoom = D2CLIENT_GetPlayerUnit()->pAct->pRoom1; pRoom; pRoom = pRoom->pRoomNext)
		{
			for(UnitAny* pMonster = pRoom->pUnitFirst; pMonster; pMonster = pMonster->pListNext)
			{
				if(pMonster->dwType == UNIT_MONSTER)
				{
					if(pMonster->dwTxtFileNo == MERC_A1 || pMonster->dwTxtFileNo == MERC_A2 || pMonster->dwTxtFileNo == MERC_A3 || pMonster->dwTxtFileNo == MERC_A5)
					{
						if(D2CLIENT_GetMonsterOwner(pMonster->dwUnitId) == pUnit->dwUnitId)
						{
							myUnit* pmyUnit = new myUnit;

							if(!pmyUnit)
								return JS_TRUE;

							pmyUnit->_dwPrivateType = PRIVATE_UNIT;
							pmyUnit->dwUnitId = pMonster->dwUnitId;
							pmyUnit->dwClassId = pMonster->dwTxtFileNo;
							pmyUnit->dwMode = NULL;
							pmyUnit->dwType = UNIT_MONSTER;
							pmyUnit->szName[0] = NULL;

							JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
							if (!jsunit)
								return JS_TRUE;

							*rval = OBJECT_TO_JSVAL(jsunit);								
						}
					}
				}
			}
		}
	}
    
	return JS_TRUE;
}

// unit.setSkill( int skillId OR String skillName, int hand [, int itemGlobalId] );
INT unit_setskill(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit setSkill");

	if(!GameReady())
		return JS_TRUE;

	WORD nSkillId = (WORD)-1;
	BOOL nHand = FALSE;
	*rval = JSVAL_FALSE;

	if(argc < 1)
		return JS_TRUE;

	if(JSVAL_IS_STRING(argv[0]))
		nSkillId = GetSkillByName(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	else if(JSVAL_IS_INT(argv[0]))
		nSkillId = (WORD)JSVAL_TO_INT(argv[0]);
	else
		return JS_TRUE;

	if(JSVAL_IS_INT(argv[1]))
		nHand = !!JSVAL_TO_INT(argv[1]);
	else
		return JS_TRUE;

	if(SetSkill(nSkillId, nHand))
		*rval = JSVAL_TRUE;

	return JS_TRUE;
}

INT my_overhead(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit overhead");

	if(!GameReady())
		return JS_TRUE;

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!pmyUnit || IsBadReadPtr(pmyUnit, sizeof(myUnit)) || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit)
		return JS_TRUE;

	if(!JSVAL_IS_NULL(argv[0]) && !JSVAL_IS_VOID(argv[0]))
	{
		CHAR *lpszText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(lpszText && lpszText[0])
		{
			OverheadMsg* pMsg = D2COMMON_GenerateOverheadMsg(NULL, lpszText, *p_D2CLIENT_OverheadTrigger);
			if(pMsg)
			{
				D2COMMON_FixOverheadMsg(pMsg, NULL);
				pUnit->pOMsg = pMsg;
			}
		}
	}

	return JS_TRUE;
}


INT unit_getItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("unit getItem");

	if(!GameReady())
		return JS_TRUE;

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!pmyUnit || IsBadReadPtr(pmyUnit, sizeof(myUnit)) || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit || !pUnit->pInventory)
		return JS_TRUE;

	if(argc == NULL || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	INT nLoc = JSVAL_TO_INT(argv[0]);

	if(nLoc)
	{
		for(UnitAny* pItem = pUnit->pInventory->pFirstItem; pItem; pItem = D2COMMON_GetNextItemFromInventory(pItem))
		{
			if(pItem->pItemData)
			{
				if(pItem->pItemData->BodyLocation == nLoc)
				{
					pmyUnit = new myUnit;

					if(!pmyUnit || IsBadReadPtr(pmyUnit, sizeof(myUnit)))
						return JS_TRUE;

					pmyUnit->_dwPrivateType = PRIVATE_UNIT;
					pmyUnit->dwUnitId = pItem->dwUnitId;
					pmyUnit->dwClassId = pItem->dwTxtFileNo;
					pmyUnit->dwMode = NULL;
					pmyUnit->dwType = UNIT_ITEM;
					pmyUnit->szName[0] = NULL;

					JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
					if (!jsunit)
						return JS_TRUE;

					*rval = OBJECT_TO_JSVAL(jsunit);

					return JS_TRUE;
				}
			}
		}
	}
	return JS_TRUE;
}

INT unit_move(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit move");

	if(!GameReady())
		return JS_TRUE;

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!pmyUnit || IsBadReadPtr(pmyUnit, sizeof(myUnit)) || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	UnitAny *pPlayer = D2CLIENT_GetPlayerUnit();

	if(!pPlayer || !pUnit)
		return JS_TRUE;

	int32 x, y;

	if(pUnit == pPlayer)
	{

		if(argc < 2) 
			return JS_TRUE;

		if(JS_ValueToInt32(cx, argv[0], &x) == JS_FALSE)
			return JS_TRUE;
		if(JS_ValueToInt32(cx, argv[1], &y) == JS_FALSE)
			return JS_TRUE;
	}
	else
	{
		x = GetUnitX(pUnit);
		y = GetUnitY(pUnit);
	}

	ClickMap(0, (WORD)x, (WORD)y, FALSE, NULL);
	Sleep(50);
	ClickMap(2, (WORD)x, (WORD)y, FALSE, NULL);
//	D2CLIENT_Move((WORD)x, (WORD)y);
	return JS_TRUE;
}

INT unit_getEnchant(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit getEnchant");

	if(!GameReady())
		return JS_TRUE;
	
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!pmyUnit || IsBadReadPtr(pmyUnit, sizeof(myUnit)) || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit || pUnit->dwType != UNIT_MONSTER)
		return JS_TRUE;

	INT nEnchant = JSVAL_TO_INT(argv[0]);

	*rval = INT_TO_JSVAL(0);

	for(INT i = 0; i < 9; i++)
		if(pUnit->pMonsterData->anEnchants[i] == nEnchant)
		{
			*rval = JSVAL_TRUE;
			break;
		}

	return JS_TRUE;
}

INT unit_getQuest(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit getQuest");

	if(!GameReady())
		return JS_TRUE;
	
	if(argc < 2 || !JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1]))
		return JS_TRUE;

	jsint nAct = JSVAL_TO_INT(argv[0]);
	jsint nQuest = JSVAL_TO_INT(argv[1]);

	*rval = INT_TO_JSVAL(D2COMMON_GetQuestFlag(D2CLIENT_GetQuestInfo(), nAct, nQuest));

	return JS_TRUE;
}

INT unit_getMinionCount(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("unit getMinionCount");

	if(!GameReady())
		return JS_TRUE;
	
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	jsint nType = JSVAL_TO_INT(argv[0]);

	myUnit *pmyUnit = (myUnit*)JS_GetPrivate(cx, obj);

	if(!pmyUnit || IsBadReadPtr(pmyUnit, sizeof(myUnit)) || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit || (pUnit->dwType != UNIT_MONSTER && pUnit->dwType != UNIT_PLAYER))
		return JS_TRUE;

	*rval = INT_TO_JSVAL(D2CLIENT_GetMinionCount(pUnit, (DWORD)nType));
	
	return JS_TRUE;
}

