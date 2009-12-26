// Spidermonkey implementation of Core.cpp
#include <io.h>
#include <windows.h>
#include <ddeml.h>
#include <cmath>

#include "js32.h"
#include "Script.h"
#include "JSCore.h"
#include "JSUnit.h"
#include "JSRoom.h"
#include "JSParty.h"
#include "JSPresetUnit.h"
#include "JSArea.h"
#include "JSExits.h"
#include "Core.h"
#include "D2Ptrs.h"
#include "CriticalSections.h"
#include "CollisionMap.h"
#include "TeleportPath.h"
#include "WalkPath.h"
#include "Control.h"
#include "Constants.h"
#include "Events.h"
#include "D2Skills.h"
#include "Helpers.h"
#include "dde.h"
#include "mpqstats.h"
#include "AreaLinker.h"
#include "ScriptEngine.h"
#include "D2BS.h"

INT my_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]))
		{
			if(!JS_ConvertValue(cx, argv[i], JSTYPE_STRING, &(argv[i])))
				THROW_ERROR(cx, obj, "Converting to string failed");

			char* Text = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			if(Text == NULL)
				THROW_ERROR(cx, obj, "Could not get string for value");

			jsrefcount depth = JS_SuspendRequest(cx);

			char* c = 0;
			while((c = strchr(Text, '%')) != 0)
				*c = (unsigned char)0xFE;

			Print(Text ? Text : "undefined");
			
			JS_ResumeRequest(cx, depth);
		}
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_delay)
{
	if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		int nDelay = JSVAL_TO_INT(argv[0]);
		if(nDelay)
		{
			jsrefcount depth = JS_SuspendRequest(cx);
			Sleep(nDelay);
			JS_ResumeRequest(cx, depth);
		}
		else
			JS_ReportWarning(cx, "delay(0) called, argument must be >= 1");
			//THROW_ERROR(cx, obj, "delay(0) called, argument must be >= 1");
	}

	return JS_TRUE;
}

INT my_load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		Script* execScript = (Script*)JS_GetContextPrivate(cx);
		ScriptState scriptState = execScript->GetState();
		if(scriptState == Command)
			scriptState = (GameReady() ? InGame : OutOfGame);

		CHAR* lpszFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(!(lpszFileName && lpszFileName[0]))
			THROW_ERROR(cx, obj, "Could not convert or empty string");
		StringReplace(lpszFileName, '/', '\\');
		if(strlen(lpszFileName) < _MAX_PATH)
		{
			CHAR lpszBuf[_MAX_PATH+_MAX_FNAME];
			sprintf_s(lpszBuf, sizeof(lpszBuf), "%s\\%s", Vars.szScriptPath, lpszFileName);
			Script* script = ScriptEngine::CompileFile(lpszBuf, scriptState);
			if(script)
			{
				CreateThread(0, 0, ScriptThread, script, 0, 0);
				*rval = JSVAL_TRUE;
			}
			else
			{
				// TODO: Should this actually be there? No notification is bad, but do we want this? maybe throw an exception?
				Print("File \"%s\" not found.", lpszFileName);
				*rval = JSVAL_FALSE;
			}
		}
		else
			THROW_ERROR(cx, obj, "File name exceeds _MAX_PATH characters");
	}

	return JS_TRUE;
}

INT my_include(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		if(script)
		{
			CHAR * lpszFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
			if(lpszFileName && strlen(lpszFileName) <= _MAX_FNAME)
			{
				CHAR lpszBuf[_MAX_PATH+_MAX_FNAME];
				sprintf_s(lpszBuf, sizeof(lpszBuf), "%s\\libs\\%s", Vars.szScriptPath, lpszFileName);
				if(_access(lpszBuf, 0) == 0)
					*rval = BOOLEAN_TO_JSVAL(script->Include(lpszBuf));
				else
					*rval = JSVAL_FALSE;
			}
			else
				*rval = JSVAL_FALSE;
		}
	}
	else
		*rval = JSVAL_FALSE;

	return JS_TRUE;
}

INT my_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 0 && (JSVAL_IS_INT(argv[0]) && JSVAL_TO_INT(argv[0]) == 1) ||
			(JSVAL_IS_BOOLEAN(argv[0]) && JSVAL_TO_BOOLEAN(argv[0]) == TRUE))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		if(script)
			script->Stop();
	}
	else
		ScriptEngine::StopAll();

	return JS_FALSE;
}

INT my_copyUnit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	if(argc >= 1 && JSVAL_IS_OBJECT(argv[0]) && !JSVAL_IS_NULL(argv[0]))
	{
		*rval = JSVAL_VOID;
		Private* myPrivate = (Private*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

		if(!myPrivate)
			return JS_TRUE;

		if(myPrivate->dwPrivateType == PRIVATE_UNIT)
		{
			myUnit* lpOldUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
			myUnit* lpUnit = new myUnit;

			if(lpUnit)
			{
				memcpy(lpUnit, lpOldUnit, sizeof(myUnit));
				JSObject* jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, lpUnit);
				if(!jsunit)
				{
					delete lpUnit;
					lpUnit = NULL;
					THROW_ERROR(cx, obj, "Couldn't copy unit");
				}

				*rval = OBJECT_TO_JSVAL(jsunit);
			}
		}
		else if(myPrivate->dwPrivateType == PRIVATE_ITEM)
		{
			invUnit* lpOldUnit = (invUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
			invUnit* lpUnit = new invUnit;

			if(lpUnit)
			{
				memcpy(lpUnit, lpOldUnit, sizeof(invUnit));
				JSObject* jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, lpUnit);
				if(!jsunit)
				{
					delete lpUnit;
					lpUnit = NULL;
					THROW_ERROR(cx, obj, "Couldn't copy unit");
				}

				*rval = OBJECT_TO_JSVAL(jsunit);
			}
		}
	}

	return JS_TRUE;
}

INT my_clickMap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	WORD nClickType = NULL, nShift = NULL, nX = NULL, nY = NULL;

	*rval = JSVAL_FALSE;
	
	if(argc < 3)
		return JS_TRUE;

	if(JSVAL_IS_INT(argv[0]))
		nClickType = (WORD)JSVAL_TO_INT(argv[0]);
	if(JSVAL_IS_INT(argv[1]))
		nShift = (WORD)JSVAL_TO_INT(argv[1]);
	if(JSVAL_IS_INT(argv[2]))
		nX = (WORD)JSVAL_TO_INT(argv[2]);
	if(JSVAL_IS_INT(argv[3]))
		nY = (WORD)JSVAL_TO_INT(argv[3]);

	if(argc == 3 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && !JSVAL_IS_NULL(argv[2]) && JSVAL_IS_OBJECT(argv[2]))
	{
		myUnit* mypUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[2]));

		if(!mypUnit || (mypUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnit = D2CLIENT_FindUnit(mypUnit->dwUnitId, mypUnit->dwType);

		if(!pUnit)
			return JS_TRUE;

		Vars.dwSelectedUnitId = NULL;
		Vars.dwSelectedUnitType = NULL;

		*rval = BOOLEAN_TO_JSVAL(ClickMap(nClickType, nX, nY, nShift, pUnit));
	}
	else if(argc > 3 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]))
	{
		*rval = BOOLEAN_TO_JSVAL(ClickMap(nClickType, nX, nY, nShift, NULL));
	}

	return JS_TRUE;
}

INT my_beep(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsint nBeepId = NULL;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nBeepId = JSVAL_TO_INT(argv[0]);

	// TODO: range check this value
	MessageBeep(nBeepId);

	return JS_TRUE;
}

INT my_acceptTrade(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	// TODO: Fix this nonsense.
	if(argc > 0 && JSVAL_TO_INT(argv[0]) == 1) // Called with a '1' it will return if we already accepted it or not
	{
		*rval = BOOLEAN_TO_JSVAL((*p_D2CLIENT_bTradeAccepted));
		return JS_TRUE;
	}
	else if(JSVAL_TO_INT(argv[0]) == 2) // Called with a '2' it will return the trade flag
	{
		*rval = INT_TO_JSVAL((*p_D2CLIENT_TradeFlag));
		return JS_TRUE;
	}
	else if(JSVAL_TO_INT(argv[0]) == 3) // Called with a '3' it will return if the 'check' is red or not
	{
		*rval = BOOLEAN_TO_JSVAL((*p_D2CLIENT_bTradeBlock));
		return JS_TRUE;
	}

	if((*p_D2CLIENT_TradeFlag) == 3 || (*p_D2CLIENT_TradeFlag) == 5 || (*p_D2CLIENT_TradeFlag) == 7)
	{
		if((*p_D2CLIENT_bTradeBlock))
		{
			// Don't operate if we can't trade anyway ...
			*rval = JSVAL_FALSE;
		}
		else if((*p_D2CLIENT_bTradeAccepted))
		{
			(*p_D2CLIENT_bTradeAccepted) = FALSE;
			D2CLIENT_CancelTrade();
			*rval = JSVAL_TRUE;
		}
		else
		{
			(*p_D2CLIENT_bTradeAccepted) = TRUE;
			D2CLIENT_AcceptTrade();
			*rval = JSVAL_TRUE;
		}
		return JS_TRUE;
	}

	THROW_ERROR(cx, obj, "Invalid parameter passed to acceptTrade!");
}

INT my_blockMinimize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 0 && (JSVAL_IS_INT(argv[0]) || JSVAL_IS_BOOLEAN(argv[0])))
		Vars.bBlockMinimize = !!JSVAL_TO_BOOLEAN(argv[0]);

	return JS_TRUE;
}

INT my_getPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	if(argc < 5)
		THROW_ERROR(cx, obj, "Not enough parameters were passed to getPath!");

	CriticalRoom myMisc;
	myMisc.EnterSection();

	*rval = JSVAL_FALSE;
	DWORD dwCount = NULL;
	POINT lpBuffer[255] = {0};
	DWORD *AreaIds = NULL;
	jsuint dwLength = 0;
	DWORD Area = 0;

	if (JSVAL_IS_OBJECT(argv[0])) {
		JSObject* pObject = JSVAL_TO_OBJECT(argv[0]);
		JS_GetArrayLength(cx, pObject, &dwLength);
		AreaIds = new DWORD[dwLength];
		jsval nVal;
		for (int n = 0; n < (INT)dwLength; n++) {
			JS_GetElement(cx, pObject, n, &nVal);
			AreaIds[n] = JSVAL_TO_INT(nVal);
		}
		Area = AreaIds[0];
	} else {
		Area = JSVAL_TO_INT(argv[0]);
	}
	POINT ptStart = { JSVAL_TO_INT(argv[1]),JSVAL_TO_INT(argv[2]) };
	POINT ptEnd = { JSVAL_TO_INT(argv[3]),JSVAL_TO_INT(argv[4]) };
	BOOL UseTele = !IsTownLevel(Area);
	BOOL Reduction = true;
	if(argc >= 6)
		UseTele = JSVAL_TO_BOOLEAN(argv[5]);
	DWORD Radius = (!IsTownLevel(Area) && UseTele) ? 35 : 20;
	if(argc >= 7)
		Radius = JSVAL_TO_INT(argv[6]);
	if(argc == 8)
		Reduction = !!JSVAL_TO_BOOLEAN(argv[7]);

	CCollisionMap g_collisionMap;

	DWORD nAreas[64] = {0};
	INT nLen = GetAreas(nAreas, 64, Area, (WORD)ptEnd.x, (WORD)ptEnd.y);

	if (JSVAL_IS_OBJECT(argv[0])) {
		if (!g_collisionMap.CreateMap(AreaIds, dwLength)) {
			*rval = JSVAL_FALSE;
			return JS_TRUE;
		}
	} else {
		if(nLen)
		{
			if(!g_collisionMap.CreateMap(nAreas, nLen))
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;	
			}			
		}
		else
			if(!g_collisionMap.CreateMap(Area))
			{
				*rval = JSVAL_FALSE;
				return JS_TRUE;	
			}
	}

	if (!g_collisionMap.IsValidAbsLocation(ptStart.x, ptStart.y) ||
		!g_collisionMap.IsValidAbsLocation(ptEnd.x, ptEnd.y))
		return JS_TRUE;

	g_collisionMap.AbsToRelative(ptStart);
	g_collisionMap.AbsToRelative(ptEnd);

	WordMatrix matrix;
	if(!g_collisionMap.CopyMapData(matrix))
		return JS_TRUE;

	g_collisionMap.MakeBlank(matrix, ptStart);
	g_collisionMap.MakeBlank(matrix, ptEnd);

	bool bFix = FALSE;

	if(UseTele)
	{
		CTeleportPath tf(matrix.GetData(), matrix.GetCX(), matrix.GetCY());
		dwCount = tf.FindTeleportPath(ptStart, ptEnd, lpBuffer, 255, Radius);
	}
	else
	{
		g_collisionMap.ThickenWalls(matrix, 1);
		CWalkPath wp(matrix.GetData(), matrix.GetCX(), matrix.GetCY());

		dwCount = (DWORD)wp.FindWalkPath(ptStart, ptEnd, lpBuffer, 255, Radius, !!Reduction);
	}

	if(dwCount > 1)
		bFix = TRUE;
	if(dwCount)
	{
		JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
		for (DWORD i = 0; i < dwCount; i++)
			g_collisionMap.RelativeToAbs(lpBuffer[i]);
		
		DWORD dwArray = NULL;
		DWORD i = 0;
		if(bFix)
			i++;

		while(i < dwCount) 
		{
			JSObject* pArrayInsert = JS_NewArrayObject(cx, NULL, NULL);

			jsval x = INT_TO_JSVAL(lpBuffer[i].x);
			jsval y = INT_TO_JSVAL(lpBuffer[i].y);

			JS_SetElement(cx, pArrayInsert, 0, &x);
			JS_SetElement(cx, pArrayInsert, 1, &y);	

			jsval aObj = OBJECT_TO_JSVAL(pArrayInsert);

			JS_SetElement(cx, pReturnArray, dwArray, &aObj);
			dwArray++;
			i++;
		}

		*rval = OBJECT_TO_JSVAL(pReturnArray); 
	}
	return JS_TRUE;
}

INT my_getCollision(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	if(argc < 3 || !JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1]) || !JSVAL_IS_INT(argv[2]))
		THROW_ERROR(cx, obj, "Invalid parameters were passed to getCollision!");

	CriticalRoom myMisc;
	myMisc.EnterSection();

	DWORD nLevelId = JSVAL_TO_INT(argv[0]);
	jsint nX = JSVAL_TO_INT(argv[1]);
	jsint nY = JSVAL_TO_INT(argv[2]);
	if (Vars.cCollisionMap.dwLevelId && Vars.cCollisionMap.dwLevelId != nLevelId)
	{
		Vars.cCollisionMap.DestroyMap();
		Vars.cCollisionMap.CreateMap(nLevelId);
	}
	
	if(Vars.cCollisionMap.IsValidAbsLocation(nX, nY))
		*rval = INT_TO_JSVAL(Vars.cCollisionMap.GetMapData(nX,nY, TRUE));

	return JS_TRUE;
}

typedef VOID __fastcall clickequip(UnitAny * pPlayer, Inventory * pIventory, INT loc);
INT my_clickItem (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	myUnit* pmyUnit = NULL;
	UnitAny* pUnit = NULL;

	//INT ScreenSize = D2GFX_GetScreenSize();

	POINT Belt[] =
	{
		{0,0}, // 0
		{1,0}, // 1
		{2,0}, // 2
		{3,0}, // 3

		{0,1}, // 4
		{1,1}, // 5
		{2,1}, // 6
		{3,1}, // 7

		{0,2}, // 8
		{1,2}, // 9
		{2,2}, // 10
		{3,2}, // 11

		{0,3}, // 12
		{1,3}, // 13
		{2,3}, // 14
		{3,3}, // 15
	};
	
	if(argc == 1 && JSVAL_IS_OBJECT(argv[0]))
	{
		pmyUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
		
		if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

		if(!pUnit)
			return JS_TRUE;

		clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_clickLocPtrs + (4 * pUnit->pItemData->BodyLocation));
		
		if(!click)
			return JS_TRUE;

		click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, pUnit->pItemData->BodyLocation);
	
		return JS_TRUE;
	}
	else if(argc == 2 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
	{
		jsint nClickType = JSVAL_TO_INT(argv[0]);
		jsint nBodyLoc = JSVAL_TO_INT(argv[1]);


		if(nClickType == NULL)
		{
			clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_clickLocPtrs + (4 * nBodyLoc));
			
			if(!click)
				return JS_TRUE;

			click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, nBodyLoc);	
		}
		// Click Merc Gear
		else if(nClickType == 4)
		{
			if(nBodyLoc == 1 || nBodyLoc == 3 || nBodyLoc == 4)
			{
				UnitAny* pMerc = D2CLIENT_GetMercUnit();

				if(pMerc)
				{
					D2CLIENT_MercItemAction(0x61, nBodyLoc);
				}
			}
		}

		return JS_TRUE;
	}
	else if(argc == 2 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_OBJECT(argv[1]))
	{
		pmyUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1]));
		
		if(!pmyUnit || (pmyUnit->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

		jsint nClickType = JSVAL_TO_INT(argv[0]);

		if(!pUnit || !(pUnit->dwType == UNIT_ITEM) || !pUnit->pItemData)
			THROW_ERROR(cx, obj, "Object is not an item!");

		INT InventoryLocation = GetItemLocation(pUnit);
		
		INT x = pUnit->pItemPath->dwPosX;
		INT y = pUnit->pItemPath->dwPosY;

		*p_D2CLIENT_CursorHoverX = x;
		*p_D2CLIENT_CursorHoverY = y;

		InventoryLayout* pLayout = NULL;

		if(nClickType == 4)
		{
			UnitAny* pMerc = D2CLIENT_GetMercUnit();

			if(pMerc)
				if(pUnit->pItemData && pUnit->pItemData->pOwner)
					if(pUnit->pItemData->pOwner->dwUnitId == pMerc->dwUnitId)
					{
						D2CLIENT_MercItemAction(0x61, pUnit->pItemData->BodyLocation);
					}

			return JS_TRUE;
		}
		else if(InventoryLocation == STORAGE_INVENTORY || InventoryLocation == STORAGE_STASH || InventoryLocation == STORAGE_CUBE)
		{
			switch(InventoryLocation)
			{
				case STORAGE_INVENTORY:
					pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;
					break;
				case STORAGE_STASH:
					pLayout = (InventoryLayout*)p_D2CLIENT_StashLayout;
					break;
				case STORAGE_CUBE:
					pLayout = (InventoryLayout*)p_D2CLIENT_CubeLayout;
					break;
			}

			x = pLayout->Left + x * pLayout->SlotPixelWidth + 10; 
			y = pLayout->Top + y * pLayout->SlotPixelHeight + 10;

			if(nClickType == NULL)
				D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, nClickType, pLayout, pUnit->pItemData->ItemLocation);
			else D2CLIENT_RightClickItem(x,y, pUnit->pItemData->ItemLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);

		}
		else if(InventoryLocation == STORAGE_BELT)
		{
			INT i = x;

			if( i < 0 || i > 0x0F)
				return JS_TRUE;

			if(D2GFX_GetScreenSize() == 2)
			{
				x = 440 + (Belt[i].x * 29);
				y = 580 - (Belt[i].y * 29);
			}
			else {
				x = 360 + (Belt[i].x * 29);
				y = 460 - (Belt[i].y * 29);
			}
			if(nClickType == NULL)
				D2CLIENT_clickBelt(x,y, (DWORD)D2CLIENT_GetPlayerUnit()->pInventory);
			else
				D2CLIENT_clickBeltRight(D2CLIENT_GetPlayerUnit()->pInventory,D2CLIENT_GetPlayerUnit(), nClickType == 1 ? FALSE : TRUE, i);
		}
		else if(D2CLIENT_GetCursorItem() == pUnit)
		{
			if(nClickType < 1 || nClickType > 12)
				return JS_TRUE;

			clickequip * click = (clickequip*)*(DWORD*)(D2CLIENT_clickLocPtrs + (4 * nClickType));

			if(!click)
				return JS_TRUE;
			
			click(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, nClickType);			
		}
	}
	else if(argc == 4)
	{
		if(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]))
		{
			jsint nButton = JSVAL_TO_INT(argv[0]);
			jsint nX = JSVAL_TO_INT(argv[1]);
			jsint nY = JSVAL_TO_INT(argv[2]);
			jsint nLoc = JSVAL_TO_INT(argv[3]);

			InventoryLayout* pLayout = NULL;

			*p_D2CLIENT_CursorHoverX = nX;
			*p_D2CLIENT_CursorHoverY = nY;

			// Fixing the location - so Diablo can handle it!
			if(nLoc != 5)
			{
				UnitAny* pItem = D2CLIENT_GetCursorItem();
				if(pItem)
				{
					ItemTxt* pTxt = D2COMMON_GetItemText(pItem->dwTxtFileNo);
					if(pTxt)
					{
						if(pTxt->ySize > 1)
							nY += 1;

						if(pTxt->xSize > 1)
							nX += 1;
					}
					
				}
			}

			//nLoc is location=: 0=inventory, 2=player trade, 3=cube, 4=stash, 5=belt
			if(nLoc == 0 || nLoc == 2 || nLoc == 3 || nLoc == 4)
			{
				switch(nLoc)
				{
					case 0:
						pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;
						break;
					case 2:
						pLayout = (InventoryLayout*)p_D2CLIENT_TradeLayout;
						break;
					case 3:
						pLayout = (InventoryLayout*)p_D2CLIENT_CubeLayout;
						break;
					case 4:
						pLayout = (InventoryLayout*)p_D2CLIENT_StashLayout;
						break;
				}

				INT	x = pLayout->Left + nX * pLayout->SlotPixelWidth + 10; 
				INT	y = pLayout->Top + nY * pLayout->SlotPixelHeight + 10;
				
				if(nButton == 0) // Left Click
					D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, 1, pLayout, nLoc);
				else if(nButton == 1) // Right Click
					D2CLIENT_RightClickItem(x,y,nLoc , D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
				else if(nButton == 2) // Shift Left Click
					D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, 5, pLayout, nLoc);
			
				return JS_TRUE;
			}
			else if(nLoc == 5) // Belt
			{

				int z = -1;

				for(UINT i = 0; i < ArraySize(Belt); i++)
				{
					if(Belt[i].x == nX && Belt[i].y == nY)
					{
						z = (int)i;
						break;
					}
				}

				if(z == -1)
					return JS_TRUE;

				INT x = NULL;
				INT y = NULL;

				if(D2GFX_GetScreenSize() == 2)
				{
					x = 440 + (Belt[z].x * 29);
					y = 580 - (Belt[z].y * 29);
				}
				else {
					x = 360 + (Belt[z].x * 29);
					y = 460 - (Belt[z].y * 29);
				}

				if(nButton == 0)
					D2CLIENT_clickBelt(x,y, (DWORD)D2CLIENT_GetPlayerUnit()->pInventory);	
				else if(nButton == 1)
					D2CLIENT_clickBeltRight(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory,
						FALSE, z);
				else if(nButton == 2)
					D2CLIENT_clickBeltRight(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory,
						TRUE, z);

				return JS_TRUE;
			}	
		}
	}

	return JS_TRUE;
}


INT my_getLocaleString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	wchar_t* wString = D2LANG_GetLocaleText((WORD)JSVAL_TO_INT(argv[0]));
	char* szTmp = UnicodeToAnsi(wString);
	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szTmp));
	delete[] szTmp;
	
	return JS_TRUE;
}

JSAPI_FUNC(my_rand)
{
	if(argc < 2 || !JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1]))
	{
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	srand(GetTickCount());

	long long seed = 0;
	if(ClientState() == ClientStateInGame)
		seed = D2GAME_D2Rand((*p_D2CLIENT_PlayerUnit)->dwSeed);
	else
		seed = rand();

	jsint high;
	jsint low;

	if(JS_ValueToInt32(cx, argv[0], &low) == JS_FALSE)
		THROW_ERROR(cx, obj, "Could not convert low value");

	if(JS_ValueToInt32(cx, argv[1], &high) == JS_FALSE)
		THROW_ERROR(cx, obj, "Could not convert high value");

	if(high > low+1)
	{
		int i = (seed % (high - low + 1)) + low;
		*rval = INT_TO_JSVAL(i);
	}
	else
		*rval = INT_TO_JSVAL(high);

	return JS_TRUE;
}

INT my_getDistance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	// TODO: Add the type of distance to the api design
	jsint nX1 = NULL;
	jsint nX2 = NULL;
	jsint nY1 = NULL;
	jsint nY2 = NULL;

	if(argc == 2 && JSVAL_IS_OBJECT(argv[0]) && JSVAL_IS_OBJECT(argv[1]))
	{
		myUnit* pUnit1 = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
		myUnit* pUnit2 = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1]));

		if(!pUnit1 || (pUnit1->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		if(!pUnit2 || (pUnit2->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnitA = D2CLIENT_FindUnit(pUnit1->dwUnitId, pUnit1->dwType);
		UnitAny* pUnitB = D2CLIENT_FindUnit(pUnit2->dwUnitId, pUnit2->dwType);

		if(!pUnitA || !pUnitB)
			return JS_TRUE;

		nX1 = GetUnitX(pUnitA);
		nY1 = GetUnitY(pUnitA);
		nX2 = GetUnitX(pUnitB);
		nY2 = GetUnitY(pUnitB);

	}
	else if(argc == 3)
	{
		if(JSVAL_IS_OBJECT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_INT(argv[2]))
		{
			myUnit* pUnit1 = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

			if(!pUnit1 || (pUnit1->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
				return JS_TRUE;	

			UnitAny* pUnitA = D2CLIENT_FindUnit(pUnit1->dwUnitId, pUnit1->dwType);

			if(!pUnitA)
				return JS_TRUE;

			nX1 = GetUnitX(pUnitA);
			nY1 = GetUnitY(pUnitA);
			nX2 = JSVAL_TO_INT(argv[1]);
			nY2 = JSVAL_TO_INT(argv[2]);
		}
		else if(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_OBJECT(argv[2]))
		{
			myUnit* pUnit1 = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[2]));

			if(!pUnit1 || (pUnit1->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
				return JS_TRUE;	

			UnitAny* pUnitA = D2CLIENT_FindUnit(pUnit1->dwUnitId, pUnit1->dwType);

			if(!pUnitA)
				return JS_TRUE;

			nX1 = GetUnitX(pUnitA);
			nY1 = GetUnitY(pUnitA);
			nX2 = JSVAL_TO_INT(argv[0]);
			nY2 = JSVAL_TO_INT(argv[1]);
		}
	}
	else if(argc == 4)
	{
		if(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]))
		{
			nX1 = JSVAL_TO_INT(argv[0]);
			nY1 = JSVAL_TO_INT(argv[1]);
			nX2 = JSVAL_TO_INT(argv[2]);
			nY2 = JSVAL_TO_INT(argv[3]);
		}
	}

	jsdouble jsdist = (jsdouble)abs(GetDistance(nX1, nY1, nX2, nY2));
	JS_NewNumberValue(cx, jsdist, rval);

	return JS_TRUE;
}

INT my_gold(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	jsint nGold = NULL;
	jsint nMode = 1;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nGold = JSVAL_TO_INT(argv[0]);

	if(argc > 1 && JSVAL_IS_INT(argv[1]))
		nMode = JSVAL_TO_INT(argv[1]);

	*p_D2CLIENT_GoldDialogAmount = nGold;
	*p_D2CLIENT_GoldDialogAction = nMode;
	D2CLIENT_PerformGoldDialogAction();

	return JS_TRUE;
}

INT my_checkCollision(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	if(argc == 3 && JSVAL_IS_OBJECT(argv[0]) && JSVAL_IS_OBJECT(argv[1]) && JSVAL_IS_INT(argv[2]))
	{
		myUnit*	pUnitA = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
		myUnit*	pUnitB = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1]));
		jsint			nBitMask = JSVAL_TO_INT(argv[2]);

		if(!pUnitA || (pUnitA->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT || !pUnitB || (pUnitB->_dwPrivateType & PRIVATE_UNIT) != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnit1 = D2CLIENT_FindUnit(pUnitA->dwUnitId, pUnitA->dwType);
		UnitAny* pUnit2 = D2CLIENT_FindUnit(pUnitB->dwUnitId, pUnitB->dwType);

		if(!pUnit1 || !pUnit2)
			return JS_TRUE;

		*rval = INT_TO_JSVAL(D2COMMON_CheckUnitCollision(pUnit1, pUnit2, nBitMask));
		return JS_TRUE;
	}

	return JS_TRUE;
}

INT my_getMercHP(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	if(D2CLIENT_GetPlayerUnit() && D2CLIENT_GetPlayerUnit()->pAct)
	{
		for(Room1* pRoom = D2CLIENT_GetPlayerUnit()->pAct->pRoom1; pRoom; pRoom = pRoom->pRoomNext)
		{
			for(UnitAny* pUnit = pRoom->pUnitFirst; pUnit; pUnit = pUnit->pListNext)
			{
				if(pUnit->dwType == UNIT_MONSTER &&
					(pUnit->dwTxtFileNo == MERC_A1 || pUnit->dwTxtFileNo == MERC_A2 ||
					pUnit->dwTxtFileNo == MERC_A3 || pUnit->dwTxtFileNo == MERC_A5) &&
					D2CLIENT_GetMonsterOwner(pUnit->dwUnitId) == D2CLIENT_GetPlayerUnit()->dwUnitId)									

				{
					*rval = (pUnit->dwMode == 12 ? JSVAL_ZERO : INT_TO_JSVAL(D2CLIENT_GetUnitHPPercent(pUnit->dwUnitId)));
					return JS_TRUE;
				}
			}
		}
	}

	return JS_TRUE;
}

INT my_getCursorType(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsint nType = NULL;
	
	if(argc > 0)
		nType = JSVAL_TO_INT(argv[0]);

	*rval = INT_TO_JSVAL(nType == 1 ? *p_D2CLIENT_ShopCursorType : *p_D2CLIENT_RegularCursorType);

	return JS_TRUE;
}

INT my_getSkillByName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		return JS_TRUE;

	CHAR *lpszText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!(lpszText && lpszText[0]))
		THROW_ERROR(cx, obj, "Could not convert string");

	for(INT i = 0; i < ArraySize(Game_Skills); i++)
	{
		if(!_strcmpi(Game_Skills[i].name, lpszText))
		{
			*rval = INT_TO_JSVAL(Game_Skills[i].skillID);
			return JS_TRUE;
		}
	}

	return JS_TRUE;
}

INT my_getSkillById(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	jsint nId = JSVAL_TO_INT(argv[0]);

	if(nId < 0 || nId > 280)
	{
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx,"Unknown"));
		return JS_TRUE;
	}

	for(UINT i = 0; i < ArraySize(Game_Skills); i++)
		if(Game_Skills[i].skillID == nId)
		{
			*rval =  STRING_TO_JSVAL(JS_NewStringCopyZ(cx, Game_Skills[i].name));
			return JS_TRUE;
		}

	return JS_TRUE;
}

INT my_getTickCount(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = INT_TO_JSVAL(GetTickCount());
	return JS_TRUE;
}

INT my_getThreadPriority(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = GetThreadPriority(GetCurrentThread());
	return JS_TRUE;
}

INT my_getTextWidthHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 2 || !JSVAL_IS_STRING(argv[0]) || !JSVAL_IS_INT(argv[1]))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CHAR* pString = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!pString)
		THROW_ERROR(cx, obj, "Could not convert string");

	POINT r = CalculateTextLen(pString, JSVAL_TO_INT(argv[1]));
	jsval x = INT_TO_JSVAL(r.x);
	jsval y = INT_TO_JSVAL(r.y);

	JSObject* pObj = NULL;
	if(argc > 2 && JSVAL_IS_BOOLEAN(argv[2]) && JSVAL_TO_BOOLEAN(argv[2]) == TRUE)
	{
		// return an object with a height/width rather than an array
		pObj = BuildObject(cx, NULL);
		if(!pObj)
			THROW_ERROR(cx, NULL, "Could not build object");
		if(JS_SetProperty(cx, pObj, "width", &x) == JS_FALSE)
			THROW_ERROR(cx, pObj, "Could not set width property");
		if(JS_SetProperty(cx, pObj, "height", &y) == JS_FALSE)
			THROW_ERROR(cx, pObj, "Could not set height property");
	}
	else
	{
		pObj = JS_NewArrayObject(cx, NULL, NULL);
		JS_SetElement(cx, pObj, 0, &x);
		JS_SetElement(cx, pObj, 1, &y);
	}
	*rval = OBJECT_TO_JSVAL(pObj);

	return JS_TRUE;
}

INT my_getTradeInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	if(argc < 1)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	jsint nMode = JSVAL_TO_INT(argv[0]);
	
	if(nMode == 0)
	{
		*rval = INT_TO_JSVAL((*p_D2CLIENT_TradeFlag));
		return JS_TRUE;
	}
	else if(nMode == 1)
	{
		char* tmp = UnicodeToAnsi((wchar_t*)(*p_D2CLIENT_RecentTradeName));
		*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, tmp));
		delete[] tmp;
		
		return JS_TRUE;
	}
	else if(nMode == 2)
	{
		*rval = INT_TO_JSVAL((*p_D2CLIENT_RecentTradeId));
		return JS_TRUE;
	}
	*rval = JSVAL_FALSE;

	return JS_TRUE;
}

INT my_getUIFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
	{
		*rval = JSVAL_VOID;
		return JS_TRUE;
	}

	jsint nUIId = JSVAL_TO_INT(argv[0]);
	*rval = BOOLEAN_TO_JSVAL(D2CLIENT_GetUIState(nUIId));

	return JS_TRUE;
}

INT my_getWaypoint(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	jsint nWaypointId = JSVAL_TO_INT(argv[0]);

	if(nWaypointId > 40)
		nWaypointId = NULL;

	*rval = BOOLEAN_TO_JSVAL(!!D2COMMON_CheckWaypoint((*p_D2CLIENT_WaypointTable), nWaypointId));

	return JS_TRUE;
}

INT my_isIncluded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CHAR* szFile = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!(szFile && szFile[0]))
		THROW_ERROR(cx, obj, "Could not convert string");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\libs\\%s", Vars.szScriptPath, szFile);
	Script* js = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(js->IsIncluded(path));

	return JS_TRUE;
}

INT my_quitGame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(GameReady())
		D2CLIENT_ExitGame();

	// give the core a chance to shut down
	Shutdown();
	TerminateProcess(GetCurrentProcess(), 0);

	return JS_TRUE;
}

INT my_quit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(GameReady())
		D2CLIENT_ExitGame();

	return JS_TRUE;
}

// I need to take a closer look at the D2CLIENT_PlaySound function
INT my_playSound(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	jsint nSoundId = JSVAL_TO_INT(argv[0]);
	D2CLIENT_PlaySound(nSoundId);

	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

INT my_version(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1)
	{
		*rval = STRING_TO_JSVAL(JS_InternString(cx, D2BS_VERSION));
		return JS_TRUE;
	}

	Print("ÿc4D2BSÿc1 ÿc3%s for Diablo II 1.12a.", D2BS_VERSION); 

	return JS_TRUE;
}

JSAPI_FUNC(my_debugLog)
{
	for(uintN i = 0; i < argc; i++)
	{
		char* msg = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
		if(!msg)
			THROW_ERROR(cx, obj, "Could not convert string");
		Log("%s", msg);
	}

	return JS_TRUE;
}

INT my_say(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]) && !JSVAL_IS_VOID(argv[i]))
		{
			CHAR *lpszText = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			if(!lpszText)
				THROW_ERROR(cx, obj, "Could not convert string");
			Say(lpszText);
		}
	}

	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

INT my_sendCopyData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 4)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	CHAR* windowClassName = NULL,* windowName = NULL,* data = NULL;
	jsint nModeId = NULL;

	if(JSVAL_IS_STRING(argv[0]))
	{
		windowClassName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(!windowClassName)
			THROW_ERROR(cx, obj, "Could not convert string");
	}
	
	if(JSVAL_IS_STRING(argv[1]))
	{
		windowName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		if(!windowName)
			THROW_ERROR(cx, obj, "Could not convert string");
	}
	
	if(JSVAL_IS_INT(argv[2]))
	{
		if(JS_ValueToInt32(cx, argv[2], &nModeId) == JS_FALSE)
			THROW_ERROR(cx, obj, "Could not convert value");
	}
	
	if(JSVAL_IS_STRING(argv[3]))
	{
		data = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
		if(!data)
			THROW_ERROR(cx, obj, "Could not convert string");
	}

	HWND hWnd = FindWindow(windowClassName, windowName);
	if(!hWnd)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	// TODO: data can be null, and strlen(NULL) crashes
	COPYDATASTRUCT aCopy = { nModeId, strlen(data)+1,data };

	INT sz = SendMessage(hWnd, WM_COPYDATA, (WPARAM)D2WIN_GetHwnd(), (LPARAM)&aCopy);
	*rval = INT_TO_JSVAL(sz);

	return JS_TRUE;
}

INT my_sendDDE(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 4 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_STRING(argv[1]) && JSVAL_IS_STRING(argv[2])
		&& JSVAL_IS_STRING(argv[3]) && JSVAL_IS_STRING(argv[4]))
		return JS_TRUE;

	jsint mode;
	if(JS_ValueToInt32(cx, argv[0], &mode) == JS_FALSE)
		THROW_ERROR(cx, obj, "Could not convert value");
	char *pszDDEServer = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	if(!strlen(pszDDEServer))
		pszDDEServer = "\"\"";
	char *pszTopic = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	if(!strlen(pszTopic))
		pszTopic = "\"\"";
	char *pszItem = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
	if(!strlen(pszItem))
		pszItem = "\"\"";
	char *pszData = JS_GetStringBytes(JS_ValueToString(cx, argv[4]));
	if(!strlen(pszData))
		pszData = "\"\"";

	char buf[1000];
	char pszDdeRet[255];
	DWORD pidInst = 0;
	HCONV hConv;
	DWORD dwTimeout = 5000;
	HDDEDATA DdeSrvData;

	int ret = DdeInitialize(&pidInst, (PFNCALLBACK) DdeCallback, APPCMD_CLIENTONLY, 0);
	if(ret != DMLERR_NO_ERROR)
	{
		sprintf_s(buf, sizeof(buf), "DdeInitialize Error: %X", ret);
		OutputDebugString(buf);
		return JS_TRUE;
	}

	HSZ hszDDEServer = DdeCreateStringHandle(pidInst, pszDDEServer, CP_WINANSI);
	HSZ hszTopic = DdeCreateStringHandle(pidInst, pszTopic, CP_WINANSI);
	HSZ hszCommand = DdeCreateStringHandle(pidInst, pszItem, CP_WINANSI);

	if(!hszDDEServer || !hszTopic || !hszCommand)
	{
		Log("Error creating DDE Handles: Server:%s, Topic:%s, Command:%s, Data:%s", pszDDEServer, pszTopic, pszItem, pszData);
		// this should never fail, so die if it does
		return JS_FALSE;
	}

	hConv = DdeConnect(pidInst, hszDDEServer, hszTopic, 0);

	switch(mode)
	{
		case 0:
			DdeSrvData = DdeClientTransaction(0, 0, hConv, hszCommand, CF_TEXT, XTYP_REQUEST, dwTimeout, 0);
			DdeGetData(DdeSrvData, (LPBYTE)pszDdeRet, sizeof(pszDdeRet), 0);
			if(pszDdeRet)
				*rval=STRING_TO_JSVAL(JS_NewStringCopyZ(cx, pszDdeRet));
			break;
		case 1:
			DdeClientTransaction((LPBYTE)pszData, strlen(pszData)+1, hConv, hszCommand, CF_TEXT, XTYP_POKE, dwTimeout, 0);
			break;
		case 2:
			DdeClientTransaction((LPBYTE)pszData, strlen(pszData)+1, hConv, 0L, 0, XTYP_EXECUTE, dwTimeout, 0);
			break;
	}

	DdeFreeStringHandle(pidInst, hszDDEServer);
	DdeFreeStringHandle(pidInst, hszTopic);
	DdeFreeStringHandle(pidInst, hszCommand);
	DdeUninitialize(pidInst);
	return JS_TRUE;
}

INT my_keystate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;
	
	*rval = BOOLEAN_TO_JSVAL(!!GetAsyncKeyState(JSVAL_TO_INT(argv[0])));

	return JS_TRUE;
}

JSAPI_FUNC(my_addEventListener)
{
	if(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_FUNCTION(cx, argv[1]))
	{
		char* evtName = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		if(evtName && strlen(evtName))
		{
			Script* self = (Script*)JS_GetContextPrivate(cx);
			self->RegisterEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])), argv[1]);
		}
		else
			THROW_ERROR(cx, obj, "Event name is invalid!");
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_removeEventListener)
{
	if(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_FUNCTION(cx, argv[1]))
	{
		char* evtName = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		if(evtName && strlen(evtName))
		{
			Script* self = (Script*)JS_GetContextPrivate(cx);
			self->UnregisterEvent(evtName, argv[1]);
		}
		else
			THROW_ERROR(cx, obj, "Event name is invalid!");
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearEvent)
{
	if(JSVAL_IS_STRING(argv[0]))
	{
		Script* self = (Script*)JS_GetContextPrivate(cx);
		self->ClearEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])));
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearAllEvents)
{
	Script* self = (Script*)JS_GetContextPrivate(cx);
	self->ClearAllEvents();
	return JS_TRUE;
}

INT my_getRoom(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	CriticalRoom cRoom;
	cRoom.EnterSection();

	if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		if(JSVAL_TO_INT(argv[0]) != NULL) // 1 Parameter, AreaId
		{
			Level* pLevel = GetLevel(JSVAL_TO_INT(argv[0]));

			if(!pLevel || !pLevel->pRoom2First)
				return JS_TRUE;

			JSObject *jsroom = BuildObject(cx, &room_class, room_methods, room_props, pLevel->pRoom2First);
			if (!jsroom)
				return JS_TRUE;

			*rval=OBJECT_TO_JSVAL(jsroom);	
			
			return JS_TRUE;
		}
		else if(JSVAL_TO_INT(argv[0]) == NULL)
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

		if(argc == 3)
			pLevel = GetLevel(JSVAL_TO_INT(argv[0]));
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

INT my_clickParty(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	*rval = JSVAL_FALSE;

	if(argc < 2 || !JSVAL_IS_OBJECT(argv[0]) || !JSVAL_IS_INT(argv[1]))
		return JS_TRUE;
	
	RosterUnit* pUnit = (RosterUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
	RosterUnit* mypUnit = *p_D2CLIENT_PlayerUnitList;

	if(!pUnit || !mypUnit)
		return JS_TRUE;

	BOOL bFound = FALSE;

	for(RosterUnit* pScan = mypUnit; pScan; pScan = pScan->pNext)
		if(pScan->dwUnitId == pUnit->dwUnitId)
			bFound = TRUE;

	if(!bFound)
		return JS_TRUE;

	jsint nMode = JSVAL_TO_INT(argv[1]);

	BnetData* pData = (*p_D2LAUNCH_BnData);

	// Attempt to loot player, check first if it's hardcore
	if(nMode == 0 && pData && !(pData->nCharFlags & PLAYER_TYPE_HARDCORE))
		return JS_TRUE;

	// Attempt to party a player who is already in party ^_~
	if(nMode == 2 && pUnit->wPartyId != 0xFFFF && mypUnit->wPartyId == pUnit->wPartyId && pUnit->wPartyId != 0xFFFF)
		return JS_TRUE;

	// don't leave a party if we are in none!
	if(nMode == 3 && pUnit->wPartyId == 0xFFFF)
		return JS_TRUE;
	else if(nMode == 3 && pUnit->wPartyId != 0xFFFF) {
		*rval = JSVAL_TRUE;
		D2CLIENT_LeaveParty();
		return JS_TRUE;	
	}

	if(nMode < 0 || nMode > 3)
		return JS_TRUE;


	if(nMode == 1)
		D2CLIENT_HostilePartyUnit(pUnit, 1);
	else
		D2CLIENT_clickParty(pUnit, nMode);

	*rval = JSVAL_TRUE;

	return JS_TRUE;
}

INT my_getParty(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	if(!GameReady())
		return JS_TRUE;

	RosterUnit* pUnit = *p_D2CLIENT_PlayerUnitList;

	if(!pUnit)
		return JS_TRUE;

	JSObject* jsUnit = BuildObject(cx, &party_class, party_methods, party_props, pUnit);
	if(!jsUnit)
		return JS_TRUE;

	*rval = OBJECT_TO_JSVAL(jsUnit);

	return JS_TRUE;
}

//presetunit = getPresetUnits( int area, int type, int classid, roomarray );
//Summary: Returns an array of objects. Type, classid, and roomarray can be null. If classid is null, 
// returns all preset units of that type in the area. If type is null, returns all preset units of all types in the area.
INT my_getPresetUnits(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	if(argc < 1)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

	Level* pLevel = GetLevel(JSVAL_TO_INT(argv[0]));

	if(!pLevel)
		THROW_ERROR(cx, obj, "getPresetUnits failed, couldn't access the level!");

	uint nClassId = NULL;
	uint nType = NULL;

	if(argc >= 2)
		nType = JSVAL_TO_INT(argv[1]);
	if(argc >= 3)
		nClassId = JSVAL_TO_INT(argv[2]);

	CriticalRoom cRoom;
	cRoom.EnterSection();

	bool bAddedRoom = FALSE;
	DWORD dwArrayCount = NULL;

	for(Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next)
	{
		bAddedRoom = FALSE;

		if(!pRoom->pPreset)
		{
			D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = TRUE;
		}
		
		for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			// Does it fit?
			if((nType == NULL || pUnit->dwType == nType) && (nClassId == NULL || pUnit->dwTxtFileNo == nClassId))
			{
				myPresetUnit* mypUnit = new myPresetUnit;

				mypUnit->dwPosX = pUnit->dwPosX;
				mypUnit->dwPosY = pUnit->dwPosY;
				mypUnit->dwRoomX = pRoom->dwPosX;
				mypUnit->dwRoomY = pRoom->dwPosY;
				mypUnit->dwType = pUnit->dwType;
				mypUnit->dwId = pUnit->dwTxtFileNo;

				JSObject* unit = BuildObject(cx, &presetunit_class, NULL, presetunit_props, mypUnit);
				if(!unit)
				{
					delete mypUnit;
					THROW_ERROR(cx, obj, "Failed to build object?");
				}

				jsval a = OBJECT_TO_JSVAL(unit);
				JS_SetElement(cx, pReturnArray, dwArrayCount, &a);

				dwArrayCount++;
			}
		}

		if(bAddedRoom)
		{
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = FALSE;			
		}
	}

	*rval = OBJECT_TO_JSVAL(pReturnArray);

	return JS_TRUE;
}

//presetunit = getPresetUnits( int area, int type, int classid, roomarray );
//Summary: Returns an array of objects. Type, classid, and roomarray can be null. If classid is null, 
// returns all preset units of that type in the area. If type is null, returns all preset units of all types in the area.
JSAPI_FUNC(my_getPresetUnit)
{
	if(!GameReady())
		return JS_TRUE;

	if(argc < 1)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	Level* pLevel = GetLevel(JSVAL_TO_INT(argv[0]));

	if(!pLevel)
		THROW_ERROR(cx, obj, "getPresetUnits failed, couldn't access the level!");

	DWORD nClassId = NULL;
	DWORD nType = NULL;

	if(argc >= 2)
		nType = JSVAL_TO_INT(argv[1]);
	if(argc >= 3)
		nClassId = JSVAL_TO_INT(argv[2]);

	CriticalRoom cRoom;
	cRoom.EnterSection();

	bool bAddedRoom = FALSE;

	for(Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next) {

		bAddedRoom = FALSE;

		if(!pRoom->pRoom1)
		{
			D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = TRUE;
		}

		for(PresetUnit* pUnit = pRoom->pPreset; pUnit; pUnit = pUnit->pPresetNext)
		{
			// Does it fit?
			if((nType == NULL || pUnit->dwType == nType) && (nClassId == NULL || pUnit->dwTxtFileNo == nClassId))
			{
				// Yes it fits! Return it
				myPresetUnit* mypUnit = new myPresetUnit;

				mypUnit->dwPosX = pUnit->dwPosX;
				mypUnit->dwPosY = pUnit->dwPosY;
				mypUnit->dwRoomX = pRoom->dwPosX;
				mypUnit->dwRoomY = pRoom->dwPosY;
				mypUnit->dwType = pUnit->dwType;
				mypUnit->dwId = pUnit->dwTxtFileNo;

				JSObject* obj = BuildObject(cx, &presetunit_class, NULL, presetunit_props, mypUnit);
				if(!obj)
				{
					delete mypUnit;
					THROW_ERROR(cx, obj, "Failed to create presetunit object");
				}

				*rval = OBJECT_TO_JSVAL(obj);
				return JS_TRUE;
			}
		}

		if(bAddedRoom)
		{
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
			bAddedRoom = FALSE;			
		}
	}

	*rval = JSVAL_FALSE;

	return JS_TRUE;
}

INT my_getArea(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	jsint nArea = GetPlayerArea();

	if(argc == 1)
	{
		if(JSVAL_IS_INT(argv[0]))
			nArea = JSVAL_TO_INT(argv[0]); 
		else
			THROW_ERROR(cx, obj, "Invalid parameter passed to getArea!");
	}

	if(nArea < 0)
		THROW_ERROR(cx, obj, "Invalid parameter passed to getArea!");
	
	Level* pLevel = GetLevel(nArea);

	if(!pLevel)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	myArea* pArea = new myArea;
	if(!pArea)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	pArea->AreaId = nArea;
	
	CriticalRoom cRoom;
	cRoom.EnterSection();

	CCollisionMap cMap;

	if(!cMap.CreateMap(nArea))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}
	
	pArea->Exits = cMap.GetLevelExits(pArea->ExitArray);
	JSObject* unit = BuildObject(cx, &area_class, NULL, area_props, pArea);
	if(!unit)
	{
		delete pArea;
		pArea = NULL;
		THROW_ERROR(cx, obj, "Failed to build area unit!");
	}

	*rval = OBJECT_TO_JSVAL(unit);

	return JS_TRUE;
}

INT my_getExits(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	*rval = JSVAL_FALSE;
	if(argc < 1 || !JSVAL_IS_OBJECT(argv[0]))
		return JS_TRUE;

	CriticalRoom cRoom;
	cRoom.EnterSection();

	myArea* pArea = (myArea*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
	if(!pArea)
		return JS_TRUE;

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
	DWORD dwArrayCount = 0;

	for(UINT i = 0; i < pArea->Exits; i++)
	{		
		myExit* pExit = new myExit;
		pExit->id	= pArea->ExitArray[i]->dwTargetLevel;
		pExit->x	= pArea->ExitArray[i]->ptPos.x;
		pExit->y	= pArea->ExitArray[i]->ptPos.y;
		pExit->type	= pArea->ExitArray[i]->dwType;
		pExit->tileid = pArea->ExitArray[i]->dwId;

		JSObject* jsUnit = BuildObject(cx, &exit_class, NULL, exit_props, pExit);
		if(!jsUnit)
		{
			delete pExit;
			pExit = NULL;
			THROW_ERROR(cx, obj, "Failed to create exit object!");
		}

		jsval a = OBJECT_TO_JSVAL(jsUnit);
		JS_SetElement(cx, pReturnArray, i, &a);
		dwArrayCount++;
	}

	*rval = OBJECT_TO_JSVAL(pReturnArray);
	return JS_TRUE;
}

INT my_getBaseStat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 2)
	{
		CHAR* szStatName = NULL;
		jsint nBaseStat = 0;
		jsint nClassId = 0;
		jsint nStat = -1;

		if(JSVAL_IS_INT(argv[0]))
			nBaseStat = JSVAL_TO_INT(argv[0]);
		else
			return JS_TRUE;

		if(JSVAL_IS_INT(argv[1]))
			nClassId = JSVAL_TO_INT(argv[1]);
		else
			return JS_TRUE;

		if(JSVAL_IS_STRING(argv[2]))
		{
			szStatName = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
			if(!szStatName)
				return JS_TRUE;
		}
		else if(JSVAL_IS_INT(argv[2]))
			nStat = JSVAL_TO_INT(argv[2]);
		else
			return JS_TRUE;

		FillBaseStat(cx, rval, nBaseStat, nClassId, nStat, szStatName);
	}

	return JS_TRUE;
}

INT my_js_strict(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc == NULL)
	{
		*rval = BOOLEAN_TO_JSVAL(((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT));
		return JS_TRUE;
	}

	if(argc == 1)
	{
		bool bFlag = ((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT);
		if(JSVAL_TO_BOOLEAN(argv[0]))
		{
			if(!bFlag)
				JS_ToggleOptions(cx, JSOPTION_STRICT);
		}
		else
		{
			if(bFlag)
				JS_ToggleOptions(cx, JSOPTION_STRICT);
		}
	}

	return JS_TRUE;
}

INT my_weaponSwitch(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	*rval = JSVAL_FALSE;

	if(!GameReady())
		return JS_TRUE;

	jsint nParameter = NULL;
	if(argc > 0)
		if(JS_ValueToInt32(cx, argv[0], &nParameter) == JS_FALSE)
			THROW_ERROR(cx, obj, "Could not convert value");
	
	if(nParameter == NULL)
	{
		// don't perform a weapon switch if current gametype is classic
		BnetData* pData = (*p_D2LAUNCH_BnData);
		if(pData)
		{
			if(!(pData->nCharFlags & PLAYER_TYPE_EXPAC))
				return JS_TRUE;
		}
		else
			THROW_ERROR(cx, obj, "Could not acquire BnData");

		BYTE aPacket[1];
		aPacket[0] = 0x60;
		D2NET_SendPacket(1, 1, aPacket);
		*rval = JSVAL_TRUE;
	}
	else
		*rval = INT_TO_JSVAL((*p_D2CLIENT_bWeapSwitch));

	return JS_TRUE;
}

INT my_transmute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	D2CLIENT_Transmute();

	return JS_TRUE;
}

INT my_getPlayerFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(!GameReady())
		return JS_TRUE;

	if(argc != 3 || !JSVAL_IS_NUMBER(argv[0]) || !JSVAL_IS_NUMBER(argv[1]) || !JSVAL_IS_NUMBER(argv[2]))
		return JS_TRUE;

	DWORD nFirstUnitId = JSVAL_TO_INT(argv[0]);
	DWORD nSecondUnitId = JSVAL_TO_INT(argv[1]);
	DWORD nFlag = JSVAL_TO_INT(argv[2]);

	*rval = INT_TO_JSVAL(D2CLIENT_TestPvpFlag(nFirstUnitId, nSecondUnitId, nFlag));

	return JS_TRUE;
}

INT my_scriptBroadcast(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc < 1)
		THROW_ERROR(cx, obj, "You must specify something to broadcast");

	ScriptBroadcastEvent(argc, argv);
	return JS_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  IniRead(filename, sectionname, keyname, default)                         //                                     
//                                                                           //
//  Oringal Credits: Jonathan Bennett <jon@hiddensoft.com>                   //
//  Embedded by: Glorfindel and Insolence                                    //              
///////////////////////////////////////////////////////////////////////////////
int my_iniread(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	DEPRECATED;

	char* pFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!pFileName)
		THROW_ERROR(cx, obj, "Could not convert string");
	char lpszBuf[MAX_PATH];
	sprintf_s(lpszBuf, sizeof(lpszBuf), "%s\\%s", Vars.szScriptPath, pFileName);

	char szBuffer[65535];        // Max ini line length is 65535 under 95

	char* pSectionName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	if(!pSectionName)
		THROW_ERROR(cx, obj, "Could not convert string (pSectionName)");
	char* pKeyName = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	if(!pKeyName)
		THROW_ERROR(cx, obj, "Could not convert string (pKeyName)");
	char* pDefault = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
	if(!pDefault)
		THROW_ERROR(cx, obj, "Could not convert string (pDefault)");

	GetPrivateProfileString(pSectionName, pKeyName, pDefault, szBuffer, 65535, lpszBuf);
	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szBuffer));
	return JS_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//  IniWrite(filename, sectionname, keyname, value)                          //
//                                                                           //
//  Original Credits: Jonathan Bennett <jon@hiddensoft.com>                  //
//  Embedded by: Insolence                                                   //
///////////////////////////////////////////////////////////////////////////////
int my_iniwrite(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	DEPRECATED;

	char* pFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!pFileName)
		THROW_ERROR(cx, obj, "Could not convert string (pFileName)");
	char lpszBuf[MAX_PATH];
	sprintf_s(lpszBuf, sizeof(lpszBuf), "%s\\%s", Vars.szScriptPath, pFileName);

	char* pSectionName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	if(!pSectionName)
		THROW_ERROR(cx, obj, "Could not convert string (pSectionName)");
	char* pKeyName = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	if(!pKeyName)
		THROW_ERROR(cx, obj, "Could not convert string (pKeyName)");
	char* pValue = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
	if(!pValue)
		THROW_ERROR(cx, obj, "Could not convert string (pValue)");

	if(WritePrivateProfileString(pSectionName, pKeyName, pValue, lpszBuf))
		WritePrivateProfileString(NULL, NULL, NULL, lpszBuf);	// Flush

	return JS_TRUE;
}

JSAPI_FUNC(my_login)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	char file[_MAX_FNAME+MAX_PATH], *profile,
		 mode[15], username[48], password[256], gateway[256], charname[24],
		 difficulty[10], maxLoginTime[10], maxCharTime[10];
	
	int loginTime = 0, charTime = 0, SPdifficulty = 0;

	if(!JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "Invalid profile specified!");

	profile = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	if(!profile)
		THROW_ERROR(cx, obj, "Could not convert string (profile)");

	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	GetPrivateProfileString(profile, "mode", "single", mode, sizeof(mode), file);
	GetPrivateProfileString(profile, "character", "ERROR", charname, sizeof(charname), file);
	GetPrivateProfileString(profile, "spdifficulty", "0", difficulty, sizeof(difficulty), file);
	GetPrivateProfileString(profile, "username", "ERROR", username, sizeof(username), file);
	GetPrivateProfileString(profile, "password", "ERROR", password, sizeof(password), file);
	GetPrivateProfileString(profile, "gateway", "ERROR", gateway, sizeof(gateway), file);		

	GetPrivateProfileString("settings", "MaxLoginTime", "5000", maxLoginTime, sizeof(maxLoginTime), file);
	GetPrivateProfileString("settings", "MaxCharSelectTime", "5000", maxCharTime, sizeof(maxCharTime), file);

	char* errorMsg ="";
	loginTime = atoi(maxLoginTime);
	charTime = atoi(maxCharTime);
	SPdifficulty = atoi(difficulty);
	Control* pControl = NULL;
	int location = 0;
	int timeout =0;
	bool loginComplete = FALSE,	skippedToBnet=TRUE;
	Vars.bBlockKeys = Vars.bBlockMouse = 1;

	while(!loginComplete)
	{
		location = OOG_GetLocation();
		switch(location)
		{
			case OOG_D2SPLASH:
				clickControl(*p_D2WIN_FirstControl);
				break;
			case OOG_CHAR_SELECT:
				if (!OOG_SelectCharacter(charname))
					 errorMsg = "invalid character name";
				break;
			case OOG_MAIN_MENU:
				if (tolower(mode[0])== 's')
					if(!clickControl(findControl(6, (char *)NULL, -1, 264,324,272,35)))	
						 errorMsg = "Failed to click the Single button?";
				if(tolower(mode[0])== 'b')
				{
					OOG_SelectGateway(gateway, 256);
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 366, 272, 35)))
						 errorMsg = "Failed to click the 'Battle.net' button?";
				}
				if(tolower(mode[0])== 'o')
				{
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 433, 272, 35)))
						errorMsg =  "Failed to click the 'Other Multiplayer' button?";
					else
						skippedToBnet = FALSE;
						// Open Battle.net
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 310, 272, 35)))
						errorMsg = "Failed to click the 'Open Battle.net' button?";
				}
				break;
			case OOG_LOGIN:
				if((tolower(mode[0])== 's') || ((tolower(mode[0]) ==  'o') && skippedToBnet))
				{
					if(!clickControl(findControl(6, "EXIT", -1,33,572,128,35)))
						errorMsg =  "Failed to click the exit button?";
					break;
				}
				pControl = findControl(1, (char *)NULL, -1, 322, 342, 162, 19);
				if(pControl)
					SetControlText(pControl, username);
				else
					errorMsg = "Failed to set the 'Username' text-edit box.";
				// Password text-edit box
				pControl = findControl(1, (char *)NULL, -1, 322, 396, 162, 19);
				if(pControl)
					SetControlText(pControl, password);
				else
					errorMsg = "Failed to set the 'Password' text-edit box.";

				pControl = findControl(6, (char *)NULL, -1, 264, 484, 272, 35);
				if(pControl)
					if(!clickControl(pControl))
						errorMsg ="Failed to click the 'Log in' button?";
				break;
			case OOG_DIFFICULTY:
				switch(SPdifficulty)
				{
				case 0:
					// normal button
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 297, 272, 35)))
						errorMsg ="Failed to click the 'Normal Difficulty' button?";
					break;
				case 1:
					// nightmare button
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 340, 272, 35)))
						errorMsg =  "Failed to click the 'Nightmare Difficulty' button?";
					break;
				case 2:
					// hell button/
					if(!clickControl(findControl(6, (char *)NULL, -1, 264, 383, 272, 35)))
						errorMsg =  "Failed to click the 'Hell Difficulty' button?";
					break;
				default:
					errorMsg =  "Invalid single player difficulty level specified!";
					break;
				}
			
			case OOG_MAIN_MENU_CONNECTING: 
			case OOG_CHARACTER_SELECT_PLEASE_WAIT:
			case OOG_PLEASE_WAIT: 
			case OOG_GATEWAY:
			case OOG_CHARACTER_SELECT_NO_CHARS: 
			case OOG_NONE:
				timeout++;
				break;
			case OOG_LOBBY: 
			case OOG_INLINE: 
			case OOG_CHAT: 
			case OOG_CREATE:
			case OOG_JOIN: 
			case OOG_LADDER: 
			case OOG_CHANNEL: 
			case OOG_GAME_EXIST: 
			case OOG_GAME_DOES_NOT_EXIST:	
				loginComplete=TRUE;
				break;
			case OOG_UNABLE_TO_CONNECT:
				errorMsg = "un-able to connect";
				break;
			case OOG_CDKEY_IN_USE:
				errorMsg = "CD-Key in use";
				break;
			case OOG_LOGIN_ERROR:
				errorMsg = "bad account or password";
				break;
			case OOG_REALM_DOWN:
				errorMsg = "Realm Down";
				break;
			default:
				errorMsg = "unhandled login location";
				break;				
		}

		if(_strcmpi(errorMsg, ""))
		{
			Vars.bBlockKeys =0;  Vars.bBlockMouse = 0;
			THROW_ERROR(cx, obj, errorMsg);						
			break;
		}

		if((timeout*100) > loginTime)
		{
			Vars.bBlockKeys =0;  Vars.bBlockMouse = 0;
			THROW_ERROR(cx, obj, "login time out");
			break;
		}

		if(GameReady())
			loginComplete = TRUE;
		
		Sleep(100);
	}
	Vars.bBlockKeys = 0; Vars.bBlockMouse = 0;

	return JS_TRUE;
}

JSAPI_FUNC(my_createGame)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]) ||
	   (argc > 1 && !JSVAL_IS_STRING(argv[1])) ||
	   (argc > 2 && !JSVAL_IS_INT(argv[2])))
		THROW_ERROR(cx, obj, "Invalid parameters specified to createGame");

	char *name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0])),
		 *pass = NULL;
	if(argc > 1)
		pass = JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));

	int diff = 3;
	if(argc > 2)
		diff = JSVAL_TO_INT(argv[2]);

	if(strlen(name) > 15 || strlen(pass) > 15)
		THROW_ERROR(cx, obj, "Invalid game name or password length");

	if(!OOG_CreateGame(name, pass, diff))
		THROW_ERROR(cx, obj, "createGame failed");

	return JS_TRUE;
}

JSAPI_FUNC(my_joinGame)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]) ||
	   (argc > 1 && !JSVAL_IS_STRING(argv[1])))
		THROW_ERROR(cx, obj, "Invalid parameters specified to joinGame");

	char *name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0])),
		 *pass = NULL;
	if(argc > 1)
		pass = JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));

	if(strlen(name) > 15 || strlen(pass) > 15)
		THROW_ERROR(cx, obj, "Invalid game name or password length");

	if(!OOG_JoinGame(name, pass))
		THROW_ERROR(cx, obj, "joinGame failed");

	return JS_TRUE;
}

JSAPI_FUNC(my_addProfile)
{
	// validate the args...
	char *profile, *mode, *gateway, *username, *password, *charname;
	if(argc != 6)
		THROW_ERROR(cx, obj, "Invalid arguments passed to addProfile");

	for(uintN i = 0; i < argc; i++)
		if(!JSVAL_IS_STRING(argv[i]))
			THROW_ERROR(cx, obj, "All arguments to addProfile must be strings!");

	profile = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	mode = JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));
	gateway = JS_GetStringBytes(JSVAL_TO_STRING(argv[2]));
	username = JS_GetStringBytes(JSVAL_TO_STRING(argv[3]));
	password = JS_GetStringBytes(JSVAL_TO_STRING(argv[4]));
	charname = JS_GetStringBytes(JSVAL_TO_STRING(argv[5]));
	if(!profile || !mode || !gateway || !username || !password || !charname)
		THROW_ERROR(cx, obj, "Failed to convert string");

	char file[_MAX_FNAME+_MAX_PATH];

	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);
	// check if the profile exists
	{
		// keep this local to this section of code so we don't cause the stack to grossly explode
		char profiles[65535];
		int count = GetPrivateProfileString(NULL, NULL, NULL, profiles, 65535, file);
		int i = 0;
		while(i < count)
		{
			if(_strcmpi(profiles+i, profile) == 0)
				THROW_ERROR(cx, obj, "Profile already exists!");
			i += strlen(profiles+i)+2;
		}
	}

	char settings[600];
	// nope, write the new one
	sprintf_s(settings, sizeof(settings),
				"mode=%s\0gateway=%s\0username=%s\0password=%s\0character=%s\0\0",
				mode, gateway, username, password, charname);

	WritePrivateProfileSection(profile, settings, file);
	return JS_TRUE;
}

JSAPI_FUNC(my_getOOGLocation)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	*rval = INT_TO_JSVAL(OOG_GetLocation());

	return JS_TRUE;
}

JSAPI_FUNC(my_getMouseCoords)
{
	bool nFlag = false, nReturn = false;

	if(argc > 0 && JSVAL_IS_INT(argv[0]) || JSVAL_IS_BOOLEAN(argv[0]))
		nFlag = !!JSVAL_TO_BOOLEAN(argv[0]);
	if(argc > 1 && JSVAL_IS_INT(argv[1]) || JSVAL_IS_BOOLEAN(argv[1]))
		nReturn = !!JSVAL_TO_BOOLEAN(argv[1]);

	JSObject* pObj = NULL;

	POINT Coords = {*p_D2CLIENT_MouseX, *p_D2CLIENT_MouseY};

	if(nFlag)
	{
		Coords.x += *p_D2CLIENT_MouseOffsetX;
		Coords.y += *p_D2CLIENT_MouseOffsetY;

		D2COMMON_AbsScreenToMap(&Coords.x, &Coords.y);
	}

	jsval jsX = INT_TO_JSVAL(Coords.x);
	jsval jsY = INT_TO_JSVAL(Coords.y);

	if(nReturn)
	{
		pObj = BuildObject(cx, NULL);
		if(!pObj)
			THROW_ERROR(cx, NULL, "Could not build object");
		if(JS_SetProperty(cx, pObj, "x", &jsX) == JS_FALSE)
			THROW_ERROR(cx, obj, "Could not set x property");
		if(JS_SetProperty(cx, pObj, "y", &jsY) == JS_FALSE)
			THROW_ERROR(cx, obj, "Could not set y property");
	}
	else
	{
		pObj = JS_NewArrayObject(cx, NULL, NULL);
		JS_SetElement(cx, pObj, 0, &jsX);
		JS_SetElement(cx, pObj, 1, &jsY);
	}

	if(!pObj)
		return JS_TRUE;

	*rval = OBJECT_TO_JSVAL(pObj);
	return JS_TRUE;
}

JSAPI_FUNC(my_submitItem)
{
	if(!GameReady())
		return JS_TRUE;

	if(UnitAny* pUnit = D2CLIENT_GetCursorItem())
	{
		D2CLIENT_submitItem(pUnit->dwUnitId);
		*rval = JSVAL_TRUE;
	}
	else
		*rval = JSVAL_FALSE;

	return JS_TRUE;
}

JSAPI_FUNC(my_getInteractedNPC)
{
	if(!GameReady())
		return JS_TRUE;

	UnitAny* pNPC = D2CLIENT_GetCurrentInteractingNPC();
	if(!pNPC)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	myUnit* pmyUnit = new myUnit; // leaked?
	if(!pmyUnit)
		return JS_TRUE;

	CHAR szName[256] = "";
	pmyUnit->_dwPrivateType = PRIVATE_UNIT;
	pmyUnit->dwClassId = pNPC->dwTxtFileNo;
	pmyUnit->dwMode = pNPC->dwMode;
	pmyUnit->dwType = pNPC->dwType;
	pmyUnit->dwUnitId = pNPC->dwUnitId;
	strcpy_s(pmyUnit->szName, sizeof(pmyUnit->szName), szName);

	JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);
	if(!jsunit)
		return JS_TRUE;

	*rval = OBJECT_TO_JSVAL(jsunit);
	return JS_TRUE;
}

