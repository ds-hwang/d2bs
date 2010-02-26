#include "JSRoom.h"
#include "CriticalSections.h"
#include "JSPresetUnit.h"
//#include "JSUnit.h"
//#include "D2Helpers.h"
//#include "D2Ptrs.h"
//#include "Room.h"

EMPTY_CTOR(room)

JSAPI_PROP(room_getProperty)
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

JSAPI_FUNC(room_getNext)
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

JSAPI_FUNC(room_getPresetUnits)
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

JSAPI_FUNC(room_getCollision)
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

JSAPI_FUNC(room_getNearby)
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
JSAPI_FUNC(room_getStat)
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
	else if(nStat == 16)
		*rval = INT_TO_JSVAL(pRoom2->pRoom1->Coll->dwSizeRoomY);

	if(bAdded)
		D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct,pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);

	return JS_TRUE;
}

JSAPI_FUNC(room_getFirst)
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

JSAPI_FUNC(room_unitInRoom)
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

JSAPI_FUNC(room_reveal)
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

JSAPI_FUNC(my_getRoom)
{
	if(!WaitForGameReady())
		THROW_ERROR(cx, "Game not ready");

	CriticalRoom cRoom;
	cRoom.EnterSection();

	if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		uint32 levelId;
		JS_ValueToECMAUint32(cx, argv[0], &levelId);
		if(levelId != 0) // 1 Parameter, AreaId
		{
			Level* pLevel = GetLevel(levelId);

			if(!pLevel || !pLevel->pRoom2First)
				return JS_TRUE;

			JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pLevel->pRoom2First);
			if (!jsroom)
				return JS_TRUE;

			*rval=OBJECT_TO_JSVAL(jsroom);	
			
			return JS_TRUE;
		}
		else if(levelId == 0)
		{
			Room1* pRoom1 = D2COMMON_GetRoomFromUnit(D2CLIENT_GetPlayerUnit());

			if(!pRoom1 || !pRoom1->pRoom2)
				return JS_TRUE;

			JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom1->pRoom2);
			if (!jsroom)
				return JS_TRUE;
			*rval=OBJECT_TO_JSVAL(jsroom);	

			return JS_TRUE;
		}
	}
	else if(argc == 3 || argc == 2) // area ,x and y
	{
		Level* pLevel = NULL;

		uint32 levelId;
		JS_ValueToECMAUint32(cx, argv[0], &levelId);

		if(argc == 3)
			pLevel = GetLevel(levelId);
		else if(D2CLIENT_GetPlayerUnit() && D2CLIENT_GetPlayerUnit()->pPath && D2CLIENT_GetPlayerUnit()->pPath->pRoom1 && D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2)
			pLevel = D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel;

		if(!pLevel || !pLevel->pRoom2First)
			return JS_TRUE;

		jsint nX = NULL;
		jsint nY = NULL;

		if(argc == 2)
		{
			nX = JSVAL_TO_INT(argv[0]);
			nY = JSVAL_TO_INT(argv[1]);
		}
		else if(argc == 3)
		{
			nX = JSVAL_TO_INT(argv[1]);
			nY = JSVAL_TO_INT(argv[2]);
		}

		if(!nX || !nY)
			return JS_TRUE;

		// Scan for the room with the matching x,y coordinates.
		for(Room2* pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
		{
			bool bAdded = FALSE;
			if(!pRoom->pRoom1)
			{
				D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
				bAdded = TRUE;
			}

			POINT RoomStart = {pRoom->pRoom1->dwXStart, pRoom->pRoom1->dwYStart };
			POINT RoomEnd = {pRoom->pRoom1->dwXStart + pRoom->pRoom1->dwXSize , pRoom->pRoom1->dwYStart + pRoom->pRoom1->dwYSize};

			for(INT x = RoomStart.x; x < RoomEnd.x; x++)
				for(INT y = RoomStart.y; y < RoomEnd.y; y++)
					if(x == nX && y == nY)
					{
						if(bAdded)
							D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
					
						JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pRoom);
						if (!jsroom)
						return JS_TRUE;

						*rval=OBJECT_TO_JSVAL(jsroom);

						return JS_TRUE;
					}

			if(bAdded)
				D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
		}

		JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pLevel->pRoom2First);
		if (!jsroom)
			return JS_TRUE;

		*rval=OBJECT_TO_JSVAL(jsroom);	

		return JS_TRUE;
	}
	else {
		JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->pRoom2First);
		if (!jsroom)
			return JS_TRUE;

		*rval=OBJECT_TO_JSVAL(jsroom);		
		return JS_TRUE;
	}

	return JS_TRUE;
}
