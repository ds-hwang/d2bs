// Fixed, rewritten and cleaned up!
#include "JSRoom.h"
#include "CriticalSections.h"
#include "JSPresetUnit.h"
#include "JSUnit.h"
#include "D2Ptrs.h"
#include "Room.h"

JSBool room_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	Room2 *pRoom2 = (Room2*)JS_GetPrivate(cx, obj);

	if(!pRoom2)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case ROOM_NUM:
			if(pRoom2->dwPresetType == 1)
				*vp = INT_TO_JSVAL(-1);
			else if(pRoom2->pType2Info)
				*vp = INT_TO_JSVAL(*pRoom2->pType2Info);
			break;
		case ROOM_XPOS:
			*vp = INT_TO_JSVAL(pRoom2->dwPosX);
			break;
		case ROOM_YPOS:
			*vp = INT_TO_JSVAL(pRoom2->dwPosY);
			break;
		case ROOM_XSIZE:
			*vp = INT_TO_JSVAL(pRoom2->dwSizeX * 5);
			break;
		case ROOM_YSIZE:
			*vp = INT_TO_JSVAL(pRoom2->dwSizeY * 5);
			break;
		case ROOM_AREA: case ROOM_LEVEL:
			if(pRoom2->pLevel)
				*vp = INT_TO_JSVAL(pRoom2->pLevel->dwLevelNo);
			break;

		case ROOM_CORRECTTOMB:
			if(pRoom2->pLevel && pRoom2->pLevel->pMisc && pRoom2->pLevel->pMisc->dwStaffTombLevel)
				*vp = INT_TO_JSVAL(pRoom2->pLevel->pMisc->dwStaffTombLevel);
			break;

		default:
			break;
	}

	return JS_TRUE;
}

JSBool room_getNext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	pRoom2 = pRoom2->pRoom2Next;

	if(!pRoom2)
	{
		JS_ClearScope(cx, obj);
		if(JS_ValueToObject(cx, JSVAL_NULL, &obj))
			*rval = JSVAL_FALSE;
	}
	else
	{
		JS_SetPrivate(cx, obj, pRoom2);
		*rval = JSVAL_TRUE;
	}

	return JS_TRUE;
}

JSBool room_getPresetUnits(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2)
		return JS_TRUE;

	DWORD nType = NULL;
	DWORD nClass = NULL;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nType = JSVAL_TO_INT(argv[0]);
	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		nClass = JSVAL_TO_INT(argv[1]);

	bool bAdded = FALSE;
	DWORD dwArrayCount = NULL;
	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

	CriticalRoom cRoom;
	cRoom.EnterSection();

	if(!pRoom2->pRoom1)
	{
		bAdded = TRUE;
		D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);	
	}


	for(PresetUnit* pUnit = pRoom2->pPreset; pUnit; pUnit = pUnit->pPresetNext)
	{
		if((pUnit->dwType == nType || nType == NULL) && (pUnit->dwTxtFileNo == nClass || nClass == NULL))
		{
			myPresetUnit* mypUnit = new myPresetUnit;

			mypUnit->dwPosX = pUnit->dwPosX;
			mypUnit->dwPosY = pUnit->dwPosY;
			mypUnit->dwRoomX = pRoom2->dwPosX;
			mypUnit->dwRoomY = pRoom2->dwPosY;
			mypUnit->dwType = pUnit->dwType;
			mypUnit->dwId = pUnit->dwTxtFileNo;

			JSObject* jsUnit = BuildObject(cx, &presetunit_class, NULL, presetunit_props, mypUnit);
			if(!jsUnit)
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;
			}

			jsval a = OBJECT_TO_JSVAL(jsUnit);
			JS_SetElement(cx, pReturnArray, dwArrayCount, &a);

			dwArrayCount++;				
		}
	}

	if(bAdded)
		D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

	*rval = OBJECT_TO_JSVAL(pReturnArray);

	return JS_TRUE;
}

JSBool room_getCollision(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2)
		return JS_TRUE;

	JSObject* jsobjy = JS_NewArrayObject(cx, NULL, NULL);

	if(!jsobjy)
		return JS_TRUE;

	bool bAdded = FALSE;
	CollMap* pCol = NULL;

	CriticalRoom cRoom;
	cRoom.EnterSection();

	if(!pRoom2->pRoom1)
	{
		bAdded = TRUE;
		D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
	}

	if(pRoom2->pRoom1)
		pCol = pRoom2->pRoom1->Coll;

	if(!pCol)
	{
		if(bAdded)
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
		return JS_TRUE;
	}

	int x = pCol->dwPosGameX - pRoom2->pLevel->dwPosX * 5;
	int y = pCol->dwPosGameY - pRoom2->pLevel->dwPosY * 5;
	int nCx = pCol->dwSizeGameX;
	int nCy = pCol->dwSizeGameY;

	int nLimitX = x + nCx;
	int nLimitY = y + nCy;

	INT nCurrentArrayY = NULL;

	WORD* p = pCol->pMapStart;

	for(int j = y; j < nLimitY; j++)		
	{
		JSObject* jsobjx = JS_NewArrayObject(cx, NULL, NULL);
		
		INT nCurrentArrayX = NULL;

		for (int i = x; i < nLimitX; i++)
		{
			jsval nNode = INT_TO_JSVAL(*p);

			if(!JS_SetElement(cx, jsobjx, nCurrentArrayX, &nNode))
			{
				if(bAdded)
					D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
				
				return JS_TRUE;
			}

			nCurrentArrayX ++;
			p++;
		}


		jsval jsu = OBJECT_TO_JSVAL(jsobjx);

		if(!JS_SetElement(cx, jsobjy, nCurrentArrayY, &jsu))
		{
			if(bAdded)
				D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

			return JS_TRUE;
		}
		
		nCurrentArrayY++;
	}


	if(bAdded)
		D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

	*rval=OBJECT_TO_JSVAL(jsobjy);

	return JS_TRUE;
}

JSBool room_getNearby(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2)
		return JS_TRUE;

	JSObject* jsobj = JS_NewArrayObject(cx, NULL, NULL);

	if(!jsobj)
		return JS_TRUE;

	DWORD dwArrayCount = NULL;

	for(UINT i = 0; i < pRoom2->dwRoomsNear; i++)
	{
		JSObject* jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom2->pRoom2Near[i]);
		if(!jsroom)
			return JS_TRUE;
		jsval jsu = OBJECT_TO_JSVAL(jsroom);

		if(!JS_SetElement(cx, jsobj, dwArrayCount, &jsu))
			return JS_TRUE;

		dwArrayCount ++;
	}

	*rval = OBJECT_TO_JSVAL(jsobj);

	return JS_TRUE;
}

// Don't know whether it works or not
JSBool room_getStat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2)
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	jsint nStat = JSVAL_TO_INT(argv[0]);

	bool bAdded = false;

	CriticalRoom cRoom;
	cRoom.EnterSection();

	if(!pRoom2->pRoom1)
	{
		bAdded = true;
		D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct,pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
	}

	if(!pRoom2->pRoom1)
		return JS_TRUE;

	if(nStat == 0) // xStart
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->dwXStart);
	else if(nStat == 1)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->dwYStart);
	else if(nStat == 2)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->dwXSize);
	else if(nStat == 3)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->dwYSize);
	else if(nStat == 4)
		*rval = INT_TO_JSVAL(pRoom2->dwPosX);
	else if(nStat == 5)
		*rval = INT_TO_JSVAL(pRoom2->dwPosY);
	else if(nStat == 6)
		*rval = INT_TO_JSVAL(pRoom2->dwSizeX);
	else if(nStat == 7)
		*rval = INT_TO_JSVAL(pRoom2->dwSizeY);
//	else if(nStat == 8)
//		*rval = INT_TO_JSVAL(pRoom2->pRoom1->dwYStart); // God knows??!!??!?!?!?!
	else if(nStat == 9)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosGameX);
	else if(nStat == 10)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosGameY);
	else if(nStat == 11)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeGameX);
	else if(nStat == 12)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeGameY);
	else if(nStat == 13)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosRoomX);
	else if(nStat == 14)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwPosGameY);
	else if(nStat == 15)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeRoomX);
	else if(nStat == 15)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeRoomY);

	if(bAdded)
		D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct,pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

	return JS_TRUE;
}

JSBool room_getFirst(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2 || !pRoom2->pLevel || !pRoom2->pLevel->pRoom2First )
		return JS_TRUE;

		JSObject* jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom2->pLevel->pRoom2First);
		if(!jsroom)
			return JS_TRUE;
		
		*rval = OBJECT_TO_JSVAL(jsroom);

	return JS_TRUE;
}

JSBool room_unitInRoom(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2 || argc < 1 || !JSVAL_IS_OBJECT(argv[0]))
		return JS_TRUE;

	myUnit* pmyUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

	if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
		return JS_TRUE;

	UnitAny* pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

	if(!pUnit)
		return JS_TRUE;
	
	Room1* pRoom1 = D2COMMON_GetRoomFromUnit(pUnit);
	
	if(!pRoom1 || !pRoom1->pRoom2)
		return JS_TRUE;

	*rval = BOOLEAN_TO_JSVAL(!!(pRoom1->pRoom2 == pRoom2));

	return JS_TRUE;
}

JSBool room_reveal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	Room2* pRoom2 = (Room2*)JS_GetPrivate(cx, obj);
	if(!pRoom2)
		return JS_TRUE;

	CriticalMisc cMisc;
	cMisc.EnterSection();
	
	BOOL bDrawPresets = false;
	if (argc == 1 && JSVAL_IS_BOOLEAN(argv[0]))
		bDrawPresets = !!JSVAL_TO_BOOLEAN(argv[0]);

	*rval = BOOLEAN_TO_JSVAL(RevealRoom(pRoom2, bDrawPresets));

	return JS_TRUE;
}

