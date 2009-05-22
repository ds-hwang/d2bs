// Spidermonkey implementation of Core.cpp
#include "js32.h"
#include "Script.h"
#include "JSCore.h"
#include "JSUnit.h"
#include "JSRoom.h"
#include "JSParty.h"
#include "JSPresetUnit.h"
#include "JSArea.h"
#include "JSExits.h"
#include "CDebug.h"
#include "Core.h"
#include "D2Ptrs.h"
#include "CriticalSections.h"
#include <io.h>
#include "CollisionMap.h"
#include "TeleportPath.h"
#include "WalkPath.h"
#include "Control.h"
#include "Constants.h"
#include "Events.h"
#include "D2Skills.h"
#include "Helpers.h"
#include <windows.h>
#include "dde.h"
#include "mpqstats.h"
#include "D2BS.h"
#include "AreaLinker.h"

#include "debugnew/debug_new.h"

INT my_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("print");

	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]))
		{
			CHAR *lpszText = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			char* c = 0;
			while((c = strchr(lpszText, '%')) != 0)
				*c = (char)0xFE;
			Print(lpszText);
		}
		else Print("undefined");
	}
	return JS_TRUE;
}

INT my_delay(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		int nDelay = JSVAL_TO_INT(argv[0]);
		for(int i = nDelay/50; i > 0; i--)
		{
			// sleep in 50ms intervals, to give the branch callback a chance to run while waiting
			Sleep(50);
			branchCallback(cx, NULL);
		}
		// sleep the remainder of the delay away
		Sleep(nDelay%50);
	}

	return JS_TRUE;
}

INT my_load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("load");

	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		Script* execScript = (Script*)JS_GetContextPrivate(cx);
		ScriptState state = execScript->GetState();
		if(state == Command)
			state = (GameReady() ? InGame : OutOfGame);

		CHAR* lpszFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

		if(lpszFileName && strlen(lpszFileName) < _MAX_PATH)
		{
			CHAR lpszBuf[_MAX_FNAME];
			sprintf(lpszBuf, "%s\\%s", Vars.szScriptPath, lpszFileName);
			Script* script = Script::CompileFile(lpszBuf, state);
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
	}

	return JS_TRUE;
}

INT my_include(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("include");

	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);

		if(script)
		{
			CHAR * lpszFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

			if(lpszFileName && strlen(lpszFileName) <= _MAX_FNAME)
			{

				CHAR lpszBuf[_MAX_PATH+_MAX_FNAME];
				sprintf(lpszBuf, "%s\\libs\\%s", Vars.szScriptPath, lpszFileName);
				if(_access(lpszBuf, 0) == 0)
					*rval = BOOLEAN_TO_JSVAL(script->Include(lpszBuf));
				else *rval = JSVAL_FALSE;
			} else
				*rval= JSVAL_FALSE;
		}
	} else *rval = JSVAL_FALSE;

	return JS_TRUE;
}

INT my_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("stop");

	if(argc > 0 && (JSVAL_IS_INT(argv[0]) && JSVAL_TO_INT(argv[0]) == 1) ||
				(JSVAL_IS_BOOLEAN(argv[0]) && JSVAL_TO_BOOLEAN(argv[0]) == TRUE))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);

		if(script)
			script->Stop();

		return JS_FALSE;
	}
	else
	{
		Script::StopAll();
		return JS_FALSE;
	}

	return JS_TRUE;
}

INT my_copyUnit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("copyUnit");

	if(argc >= 1 && JSVAL_IS_OBJECT(argv[0]) && !JSVAL_IS_NULL(argv[0]))
	{
		*rval = JSVAL_VOID;
		Private* myPrivate = (Private*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));

		if(myPrivate && myPrivate->dwPrivateType == PRIVATE_UNIT)
		{
			myUnit* lpOldUnit = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
			myUnit* lpUnit = new myUnit;

			if(lpUnit)
			{
				memcpy(lpUnit, lpOldUnit, sizeof(myUnit));
				//JS_SetContextThread(cx);
				//JS_BeginRequest(cx);
				JSObject* jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, lpUnit);
				//JS_EndRequest(cx);
				//JS_SetContextThread(cx);
				if(!jsunit)
				{
					delete lpUnit;
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
	CDebug cDbg("clickMap");

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

		if(!mypUnit || IsBadReadPtr(mypUnit, sizeof(myUnit)) || mypUnit->_dwPrivateType != PRIVATE_UNIT) // Check if the object is valid and if it's a unit object
			return JS_TRUE;

		UnitAny* pUnit = D2CLIENT_FindUnit(mypUnit->dwUnitId, mypUnit->dwType);

		if(!pUnit)
			return JS_TRUE;

		Vars.dwSelectedUnitId = NULL;
		Vars.dwSelectedUnitType = NULL;

		*rval = JSVAL_TO_BOOLEAN(ClickMap(nClickType, nX, nY, nShift, pUnit));
	}
	else if(argc > 3 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]))
	{
		*rval = JSVAL_TO_BOOLEAN(ClickMap(nClickType, nX, nY, nShift, NULL));
	}

	return JS_TRUE;
}

INT my_beep(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("beep");

	jsint nBeepId = NULL;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nBeepId = JSVAL_TO_INT(argv[0]);

	// TODO: range check this value
	MessageBeep(nBeepId);

	return JS_TRUE;
}

INT my_acceptTrade(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("acceptTrade");

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
			*rval = BOOLEAN_TO_JSVAL(FALSE);
			return JS_TRUE;
		}
		else if((*p_D2CLIENT_bTradeAccepted))
		{
			(*p_D2CLIENT_bTradeAccepted) = FALSE;
			D2CLIENT_CancelTrade();
			*rval = BOOLEAN_TO_JSVAL(TRUE);
			return JS_TRUE;
		}
		else
		{
			(*p_D2CLIENT_bTradeAccepted) = TRUE;
			D2CLIENT_AcceptTrade();
			*rval = BOOLEAN_TO_JSVAL(TRUE);
			return JS_TRUE;
		}
	}

	THROW_ERROR(cx, obj, "Invalid parameter passed to acceptTrade!");

	*rval = BOOLEAN_TO_JSVAL(FALSE);
	return JS_TRUE;
}

INT my_blockMinimize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("blockMinimize");

	if(argc > 0 && (JSVAL_IS_INT(argv[0]) || JSVAL_IS_BOOLEAN(argv[0])))
		Vars.bBlockMinimize = !!JSVAL_TO_BOOLEAN(argv[0]);

	return JS_TRUE;
}

INT my_getPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("getPath");

	if(!GameReady())
		return JS_TRUE;


	if(argc < 5)
	{
		THROW_ERROR(cx, obj, "Not enough parameters were passed to getPath!");
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	CriticalRoom myMisc;
	myMisc.EnterSection();

	*rval = BOOLEAN_TO_JSVAL(FALSE);
	DWORD dwCount = NULL;
	POINT lpBuffer[255] = {0};
	DWORD *AreaIds;
	jsuint dwLength = 0;
	DWORD Area;

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
	BOOL UseTele = IsTownLevel(Area);
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
			*rval = BOOLEAN_TO_JSVAL(false);
			return JS_TRUE;
		}
	} else {
		if(nLen)
		{
			if(!g_collisionMap.CreateMap(nAreas, nLen))
			{
				*rval = BOOLEAN_TO_JSVAL(FALSE);
				return JS_TRUE;	
			}			
		}
		else
			if(!g_collisionMap.CreateMap(Area))
			{
				*rval = BOOLEAN_TO_JSVAL(FALSE);
				return JS_TRUE;	
			}
	}

	if (!g_collisionMap.IsValidAbsLocation(ptStart.x, ptStart.y) ||
		!g_collisionMap.IsValidAbsLocation(ptEnd.x, ptEnd.y))
		return JS_TRUE;

	g_collisionMap.AbsToRelative(ptStart);
	g_collisionMap.AbsToRelative(ptEnd);

	WordMatrix matrix;
	if (!g_collisionMap.CopyMapData(matrix))
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
	if(dwCount) {
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
	CDebug cDbg("getCollision");

	if(!GameReady())
		return JS_TRUE;

	if(argc < 3 || !JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1]) || !JSVAL_IS_INT(argv[2]))
	{
		THROW_ERROR(cx, obj, "Invalid parameters were passed to getCollision!");
		return JS_TRUE;
	}

	CriticalRoom myMisc;
	myMisc.EnterSection();

	jsint nLevelId = JSVAL_TO_INT(argv[0]);
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
	CDebug cDbg("clickItem");

	if(!GameReady())
		return JS_TRUE;

	myUnit* pmyUnit = NULL;
	UnitAny* pUnit = NULL;

	//INT ScreenSize = D2GFX_GetScreenSize();
	
		POINT Belt[] = {
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
		
		if(!pmyUnit || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
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
		else if(nClickType == 3)
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
		
		if(!pmyUnit || pmyUnit->_dwPrivateType != PRIVATE_UNIT)
			return JS_TRUE;

		pUnit = D2CLIENT_FindUnit(pmyUnit->dwUnitId, pmyUnit->dwType);

		jsint nClickType = JSVAL_TO_INT(argv[0]);

		if(!pUnit || !(pUnit->dwType == UNIT_ITEM) || !pUnit->pItemData)
		{
			JS_ReportError(cx, "Object not an item");
			return JS_TRUE;
		}

		INT InventoryLocation = GetItemLocation(pUnit);
		
		INT x = pUnit->pItemPath->dwPosX;
		INT y = pUnit->pItemPath->dwPosY;

		InventoryLayout* pLayout = NULL;

		if(nClickType == 3)
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
		else if(InventoryLocation == STORAGE_INVENTORY)
		{
			//D2CLIENT_RightClickItem(x,y,nLoc , D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
			pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;

			x = pLayout->Left + x * pLayout->SlotPixelWidth + 10; 
			y = pLayout->Top + y * pLayout->SlotPixelHeight + 10;

			if(nClickType == NULL)
				D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, nClickType, pLayout, pUnit->pItemData->ItemLocation);
			else D2CLIENT_RightClickItem(x,y, pUnit->pItemData->ItemLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
		}
		else if(InventoryLocation == STORAGE_STASH)
		{
			pLayout = (InventoryLayout*)p_D2CLIENT_StashLayout;

			x = pLayout->Left + x * pLayout->SlotPixelWidth + 10; 
			y = pLayout->Top + y * pLayout->SlotPixelHeight + 10;

			if(nClickType == NULL)
				D2CLIENT_LeftClickItem(D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory, x, y, nClickType, pLayout, pUnit->pItemData->ItemLocation);
			else D2CLIENT_RightClickItem(x,y, pUnit->pItemData->ItemLocation, D2CLIENT_GetPlayerUnit(), D2CLIENT_GetPlayerUnit()->pInventory);
		}
		else if(InventoryLocation == STORAGE_CUBE)
		{
			pLayout = (InventoryLayout*)p_D2CLIENT_CubeLayout;

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

			//location=: 0=inventory, 2=player trade, 3=cube, 4=stash, 5=belt
			if(nLoc == 0) // Inventory
			{
				pLayout = (InventoryLayout*)p_D2CLIENT_InventoryLayout;

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
			else if(nLoc == 2) // Player Trade
			{
				pLayout = (InventoryLayout*)p_D2CLIENT_TradeLayout;

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
			else if(nLoc == 3) // Cube
			{
				pLayout = (InventoryLayout*)p_D2CLIENT_CubeLayout;

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
			else if(nLoc == 4) // Stash
			{
				pLayout = (InventoryLayout*)p_D2CLIENT_StashLayout;

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

				UINT z = -1;

				for(UINT i = 0; i < ArraySize(Belt); i++)
				{
					if(Belt[i].x == nX && Belt[i].y == nY)
					{
						z = i;
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
	CDebug cDbg("getLocaleString");

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;

	wchar_t* wString = D2LANG_GetLocaleText((WORD)JSVAL_TO_INT(argv[0]));
	char* szTmp = UnicodeToAnsi(wString);
	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szTmp));
	delete[] szTmp;
	
	return JS_TRUE;
}

INT my_rnd(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	// TODO: Re-evaluate why the hell we even have this function... it exists in the standard classes via Math
	CDebug cDbg("rnd");

	if(argc < 2 || !JSVAL_IS_INT(argv[0]) || !JSVAL_IS_INT(argv[1]))
	{
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	INT seed = rand();

	jsint high;
	jsint low;

	JS_ValueToInt32(cx, argv[0], &low);
	JS_ValueToInt32(cx, argv[1], &high);

	if (high > low+1) {
		int i = seed%((high-1)-low) + low+1;
		*rval = INT_TO_JSVAL(i);
	} else *rval = INT_TO_JSVAL(high);

	return JS_TRUE;
}

INT my_getDistance(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("getDistance");

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

		if(!pUnit1 || pUnit1->_dwPrivateType != PRIVATE_UNIT)
			return JS_TRUE;

		if(!pUnit2 || pUnit2->_dwPrivateType != PRIVATE_UNIT)
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

			if(!pUnit1 || pUnit1->_dwPrivateType != PRIVATE_UNIT)
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

			if(!pUnit1 || pUnit1->_dwPrivateType != PRIVATE_UNIT)
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

	INT nDistance = GetDistance(nX1, nY1, nX2, nY2);

	*rval = INT_TO_JSVAL(nDistance);

	return JS_TRUE;
}

INT my_gold(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("gold");

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
	CDebug cDbg("checkCollision");

	if(!GameReady())
		return JS_TRUE;

	if(argc == 3 && JSVAL_IS_OBJECT(argv[0]) && JSVAL_IS_OBJECT(argv[1]) && JSVAL_IS_INT(argv[2]))
	{
		myUnit*	pUnitA = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
		myUnit*	pUnitB = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1]));
		jsint			nBitMask = JSVAL_TO_INT(argv[2]);

		if(!pUnitA || pUnitA->_dwPrivateType != PRIVATE_UNIT || !pUnitB || pUnitB->_dwPrivateType != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnit1 = D2CLIENT_FindUnit(pUnitA->dwUnitId, pUnitA->dwType);
		UnitAny* pUnit2 = D2CLIENT_FindUnit(pUnitB->dwUnitId, pUnitB->dwType);

		if(!pUnit1 || !pUnit2)
			return JS_TRUE;

		*rval = INT_TO_JSVAL(D2COMMON_CheckUnitCollision(pUnit1, pUnit2, nBitMask));
		return JS_TRUE;
	}
	/*
	if(argc == 4 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) && JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]))
	{
		jsint nLevel = JSVAL_TO_INT(argv[0]);
		jsint nX = JSVAL_TO_INT(argv[1]);
		jsint nY = JSVAL_TO_INT(argv[2]);
		jsint nBitmask = JSVAL_TO_INT(argv[3]);

		CriticalRoom cRoom;
		cRoom.EnterSection();

		Level* pLevel = GetLevel(nLevel);
		
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
						*rval = INT_TO_JSVAL(D2COMMON_CheckCollision(pRoom->pRoom1, nX, nY, nBitmask));

						if(bAdded)
							D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
						return JS_TRUE;
					}

			if(bAdded)
				D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pLevel->dwLevelNo, pRoom->dwPosX, pRoom->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
		}	
	}
	else
	if(argc == 3 && JSVAL_IS_OBJECT(argv[0]) && JSVAL_IS_OBJECT(argv[1]) && JSVAL_IS_INT(argv[2]))
	{

		myUnit*	pUnitA = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0]));
		myUnit*	pUnitB = (myUnit*)JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1]));
		jsint			nBitMask = JSVAL_TO_INT(argv[2]);

		if(!pUnitA || pUnitA->_dwPrivateType != PRIVATE_UNIT || !pUnitB || pUnitB->_dwPrivateType != PRIVATE_UNIT)
			return JS_TRUE;

		UnitAny* pUnit1 = D2CLIENT_FindUnit(pUnitA->dwUnitId, pUnitA->dwType);
		UnitAny* pUnit2 = D2CLIENT_FindUnit(pUnitB->dwUnitId, pUnitB->dwType);

		if(!pUnit1 || !pUnit2)
			return JS_TRUE;

		*rval = INT_TO_JSVAL(D2COMMON_CheckUnitCollision(pUnit1, pUnit2, nBitMask));
		Log("Exit Collision check");
		return JS_TRUE;
	}
*/
	return JS_TRUE;
}

INT my_getMercHP(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("getMercHP");

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
					*rval = INT_TO_JSVAL(D2CLIENT_GetUnitHPPercent(pUnit->dwUnitId));
					return JS_TRUE;
				}
			}
		}
	}

	return JS_TRUE;
}

INT my_getCursorType(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getCursorType");

	jsint nType = NULL;
	
	if(argc > 0)
		nType = JSVAL_TO_INT(argv[0]);

	*rval = INT_TO_JSVAL(nType == 1 ? *p_D2CLIENT_ShopCursorType : *p_D2CLIENT_RegularCursorType);

	return JS_TRUE;
}

INT my_getSkillByName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getSkillByName");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
		return JS_TRUE;

	CHAR *lpszText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

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
	CDebug cDbg("getSkillById");

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
	CDebug cDbg("getTickCount");

	*rval = INT_TO_JSVAL(GetTickCount());
	return JS_TRUE;
}

INT my_getThreadPriority(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getThreadPriority");

	*rval = GetThreadPriority(GetCurrentThread());
	return JS_TRUE;
}

INT my_getTextWidthHeight(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getTextWidthHeight");

	if(argc < 2 || !JSVAL_IS_STRING(argv[0]) || !JSVAL_IS_INT(argv[1]))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	CHAR* pString = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

	POINT r = CalculateTextLen(pString, JSVAL_TO_INT(argv[1]));
	jsval x = INT_TO_JSVAL(r.x);
	jsval y = INT_TO_JSVAL(r.y);

	JSObject* pObj = JS_NewArrayObject(cx, NULL, NULL);
	JS_SetElement(cx, pObj, 0, &x);
	JS_SetElement(cx, pObj, 1, &y);

	*rval = OBJECT_TO_JSVAL(pObj);

	return JS_TRUE;
}

INT my_getTradeInfo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("getTradeInfo");

	if(!GameReady())
		return JS_TRUE;

	if(argc < 1)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
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
	*rval = BOOLEAN_TO_JSVAL(FALSE);

	return JS_TRUE;
}

INT my_getUIFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getUIFlag");

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
	CDebug cDbg("getWaypoint");

	if(!GameReady())
		return JS_TRUE;

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	jsint nWaypointId = JSVAL_TO_INT(argv[0]);

	if(nWaypointId > 40)
		nWaypointId = NULL;

	*rval = BOOLEAN_TO_JSVAL(D2COMMON_CheckWaypoint((*p_D2CLIENT_WaypointTable), nWaypointId));

	return JS_TRUE;
}

INT my_isIncluded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("isIncluded");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}
	
	CHAR* szFile = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

	Script* js = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(js->IsIncluded(szFile));

	return JS_TRUE;
}

INT my_quitGame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("quitGame");

	if(D2CLIENT_GetPlayerUnit())
		D2CLIENT_ExitGame();

	TerminateProcess(GetCurrentProcess(), -1);

	return JS_TRUE;
}

INT my_quit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("quit");

	if(D2CLIENT_GetPlayerUnit())
		D2CLIENT_ExitGame();

	return JS_TRUE;
}

// I need to take a closer look at the D2CLIENT_PlaySound function
INT my_playSound(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("playSound");

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	jsint nSoundId = JSVAL_TO_INT(argv[0]);
	D2CLIENT_PlaySound(nSoundId);

	*rval = BOOLEAN_TO_JSVAL(TRUE);

	return JS_TRUE;
}

INT my_version(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("version");

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
	CDebug cDbg("debugLog");

	for(uintN i = 0; i < argc; i++)
	{
		// TODO: Filter out %'s so they can be written to the log
		char* msg = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
		Log(msg);
	}

	return JS_TRUE;
}

INT my_GC(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("runGC");
	CriticalRoom cRoom;
	CriticalMisc cMisc;

	cRoom.EnterSection();
	cMisc.EnterSection();

	JS_YieldRequest(cx);

	return JS_TRUE;
}

INT my_say(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("say");

	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]) && !JSVAL_IS_VOID(argv[i]))
		{
			CHAR *lpszText = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			Say(lpszText);
		}
	}

	*rval = BOOLEAN_TO_JSVAL(TRUE);

	return JS_TRUE;
}

INT my_sendCopyData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sendCopyData");

	if(argc < 4)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	CHAR* windowClassName = NULL,* windowName = NULL,* data = NULL;
	jsint nModeId = NULL;

	if(JSVAL_IS_STRING(argv[0]))
		windowClassName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(JSVAL_IS_STRING(argv[1]))
		windowName		= JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	if(JSVAL_IS_INT(argv[2]))
		JS_ValueToInt32(cx, argv[2], &nModeId);
	if(JSVAL_IS_STRING(argv[3]))
		data			= JS_GetStringBytes(JS_ValueToString(cx, argv[3]));

	HWND hWnd = FindWindow(windowClassName, windowName);

	if(!hWnd)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	COPYDATASTRUCT aCopy = { nModeId, strlen(data)+1,data };

	INT sz = SendMessage(hWnd, WM_COPYDATA, (WPARAM)D2WIN_GetHwnd(), (LPARAM)&aCopy);
	*rval = INT_TO_JSVAL(sz);

	return JS_TRUE;
}

INT my_sendDDE(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sendDDE");

	if(argc > 4 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_STRING(argv[1]) && JSVAL_IS_STRING(argv[2])
		&& JSVAL_IS_STRING(argv[3]) && JSVAL_IS_STRING(argv[4]))
		return JS_TRUE;

	jsint mode;
	JS_ValueToInt32(cx, argv[0], &mode);
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
	if(ret != DMLERR_NO_ERROR) {
		sprintf(buf, "DdeInitialize Error: %X", ret);
		OutputDebugString(buf);
		return JS_TRUE;
	}

	HSZ hszDDEServer = DdeCreateStringHandle(pidInst, pszDDEServer, CP_WINANSI);
	HSZ hszTopic = DdeCreateStringHandle(pidInst, pszTopic, CP_WINANSI);
	HSZ hszCommand = DdeCreateStringHandle(pidInst, pszItem, CP_WINANSI);

	if(!hszDDEServer || !hszTopic || !hszCommand) {
		sprintf(buf, "Error creating DDE Handles: Server:%s, Topic:%s, Command:%s, Data:%s", pszDDEServer, pszTopic, pszItem, pszData);
		OutputDebugString(buf);
	}

	hConv = DdeConnect(pidInst, hszDDEServer, hszTopic, 0);

	switch(mode) {
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
	CDebug cDbg("keystate");

	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;
	
	*rval = BOOLEAN_TO_JSVAL(!!GetAsyncKeyState(JSVAL_TO_INT(argv[0])));

	return JS_TRUE;
}

JSAPI_FUNC(my_addEventListener)
{
	CDebug cDbg("addEventListener");
	if(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_FUNCTION(cx, argv[1]))
	{
		Script* self = (Script*)JS_GetContextPrivate(cx);
		self->RegisterEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])), argv[1]);
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_removeEventListener)
{
	CDebug cDbg("removeEventListener");
	if(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_FUNCTION(cx, argv[1]))
	{
		Script* self = (Script*)JS_GetContextPrivate(cx);
		self->UnregisterEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])), argv[1]);
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearEvent)
{
	CDebug cDbg("clearEvent");
	if(JSVAL_IS_STRING(argv[0]))
	{
		Script* self = (Script*)JS_GetContextPrivate(cx);
		self->ClearEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])));
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearAllEvents)
{
	CDebug cDbg("clearAllEvents");
	Script* self = (Script*)JS_GetContextPrivate(cx);
	self->ClearAllEvents();
	return JS_TRUE;
}

INT my_getRoom(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getRoom");

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
	CDebug cDbg("clickParty");

	if(!GameReady())
		return JS_TRUE;

	*rval = BOOLEAN_TO_JSVAL(FALSE);

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
		*rval = BOOLEAN_TO_JSVAL(TRUE);
		D2CLIENT_LeaveParty();
		return JS_TRUE;	
	}

	if(nMode < 0 || nMode > 3)
		return JS_TRUE;


	if(nMode == 1)
		D2CLIENT_HostilePartyUnit(pUnit, 1);
	else
		D2CLIENT_clickParty(pUnit, nMode);

	*rval = BOOLEAN_TO_JSVAL(TRUE);

	return JS_TRUE;
}

INT my_getParty(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{	
	CDebug cDbg("getParty");

	if(!GameReady())
		return JS_TRUE;

	RosterUnit* pUnit = *p_D2CLIENT_PlayerUnitList;

	if(!pUnit)
		return JS_TRUE;

	JSObject* jsUnit = JS_NewObject(cx, &party_class, NULL, NULL);
	if(!jsUnit)
		return JS_TRUE;

	if(!JS_DefineProperties(cx, jsUnit, party_props))
		return JS_TRUE;
	if(!JS_DefineFunctions(cx, jsUnit, party_methods)) 
		return JS_TRUE;
	
	JS_SetPrivate(cx, jsUnit, pUnit);

	*rval = OBJECT_TO_JSVAL(jsUnit);

	return JS_TRUE;
}

//presetunit = getPresetUnits( int area, int type, int classid, roomarray );
//Summary: Returns an array of objects. Type, classid, and roomarray can be null. If classid is null, 
// returns all preset units of that type in the area. If type is null, returns all preset units of all types in the area.
INT my_getPresetUnits(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getPresetUnits");

	if(!GameReady()) return JS_TRUE;


	if(argc < 1)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

	Level* pLevel = GetLevel(JSVAL_TO_INT(argv[0]));

	if(!pLevel)
		THROW_ERROR(cx, obj, "getPresetUnits failed, couldn't access the level!");

	jsint nClassId = NULL;
	jsint nType = NULL;

	if(argc >= 2)
	{
		nType = JSVAL_TO_INT(argv[1]);
	}
	if(argc >= 3)
	{
		nClassId = JSVAL_TO_INT(argv[2]);
	}

	CriticalRoom cRoom;
	cRoom.EnterSection();

	bool bAddedRoom = FALSE;
	DWORD dwArrayCount = NULL;

	for(Room2 *pRoom = pLevel->pRoom2First; pRoom; pRoom = pRoom->pRoom2Next) {
		
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
				JS_SetContextThread(cx);

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
	CDebug cDbg("getPresetUnit");

	if(!GameReady()) return JS_TRUE;


	if(argc < 1)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	Level* pLevel = GetLevel(JSVAL_TO_INT(argv[0]));

	if(!pLevel)
	{
		THROW_ERROR(cx, obj, "getPresetUnits failed, couldn't access the level!");
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	jsint nClassId = NULL;
	jsint nType = NULL;

	if(argc >= 2)
	{
		nType = JSVAL_TO_INT(argv[1]);
	}
	if(argc >= 3)
	{
		nClassId = JSVAL_TO_INT(argv[2]);
	}

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
				JS_SetContextThread(cx);
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
	CDebug cDbg("getArea");

	if(!GameReady())
		return JS_TRUE;

	jsint nArea = GetPlayerArea();

	if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		nArea = JSVAL_TO_INT(argv[0]); 
	}
	else
	{
		THROW_ERROR(cx, obj, "Invalid parameter passed to getArea!");
		return JS_TRUE;
	}
	
	Level* pLevel = GetLevel(nArea);
	
	if(!pLevel)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}


	myArea* pArea = new myArea;

	if(!pArea)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	pArea->AreaId = nArea;
	
	CriticalRoom cRoom;
	cRoom.EnterSection();

	CCollisionMap cMap;

	if(!cMap.CreateMap(nArea))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}
	
	pArea->Exits = cMap.GetLevelExits(pArea->ExitArray);
	JSObject* unit = BuildObject(cx, &area_class, NULL, area_props, pArea);
/*	JS_SetContextThread(cx);
	JSObject* jsUnit = JS_NewObject(cx, &area_class, NULL, NULL);

	if(!jsUnit)
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;		
	}

	if(!JS_DefineProperties(cx, jsUnit, area_props))
	{
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	JS_SetPrivate(cx, jsUnit, pArea);*/

	*rval = OBJECT_TO_JSVAL(unit);

	return JS_TRUE;
}

INT my_getExits(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getExits");

	if(!GameReady())
		return JS_TRUE;

	*rval = BOOLEAN_TO_JSVAL(FALSE);
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
		JSObject* jsUnit = JS_NewObject(cx, &exit_class, NULL, NULL);
		if(!JS_DefineProperties(cx, jsUnit, exit_props))
			return JS_TRUE;
		
		myExit* pExit = new myExit;
		pExit->id	= pArea->ExitArray[i]->dwTargetLevel;
		pExit->x	= pArea->ExitArray[i]->ptPos.x;
		pExit->y	= pArea->ExitArray[i]->ptPos.y;
		pExit->type	= pArea->ExitArray[i]->dwType;
		pExit->tileid = pArea->ExitArray[i]->dwId;

		JS_SetPrivate(cx, jsUnit, pExit);

		jsval a = OBJECT_TO_JSVAL(jsUnit);
		JS_SetElement(cx, pReturnArray, i, &a);
		dwArrayCount++;
	}



	*rval = OBJECT_TO_JSVAL(pReturnArray);

	return JS_TRUE;
}

INT my_getBaseStat(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(Vars.bDebug)
		CDebug cDbg("getBaseStat");

	if(argc > 2)
	{
		CHAR* szStatName = NULL;
		jsint nBaseStat = 0;
		jsint nClassId = 0;
		jsint nStat = -1;

		if(JSVAL_IS_INT(argv[0]))
			nBaseStat = JSVAL_TO_INT(argv[0]);
		else return JS_TRUE;

		if(JSVAL_IS_INT(argv[1]))
			nClassId = JSVAL_TO_INT(argv[1]);
		else return JS_TRUE;

		if(JSVAL_IS_STRING(argv[2]))
			szStatName = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
		else if(JSVAL_IS_INT(argv[2]))
			nStat = JSVAL_TO_INT(argv[2]);
		else return JS_TRUE;

		FillBaseStat(cx, rval, nBaseStat, nClassId, nStat, szStatName);
	}

	return JS_TRUE;
}

INT my_js_strict(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("js_strict");

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
	CDebug cDbg("weaponSwitch");

	if(!GameReady())
		return JS_TRUE;

	jsint nParameter = NULL;
	if(argc > 0)
		JS_ValueToInt32(cx, argv[0], &nParameter);
	
	if(nParameter == NULL)
	{
		// don't perform a weapon switch if current gametype is classic
		BnetData* pData = (*p_D2LAUNCH_BnData);
		if(pData)
		{
			if(!(pData->nCharFlags & PLAYER_TYPE_EXPAC))
			{
				*rval = BOOLEAN_TO_JSVAL(FALSE);
				return JS_TRUE;
			}
		}

		BYTE aPacket[1];
		aPacket[0] = 0x60;
		D2NET_SendPacket(1, 1, aPacket);
		*rval = BOOLEAN_TO_JSVAL(TRUE);
	}
	else
	{
		*rval = INT_TO_JSVAL((*p_D2CLIENT_bWeapSwitch));
	}

	return JS_TRUE;
}

INT my_transmute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("transmute");

	if(!GameReady())
		return JS_TRUE;

	D2CLIENT_Transmute();

	return JS_TRUE;
}

INT my_getPlayerFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getPlayerFlag");

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
	CDebug cDbg("scriptBroadcast");

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
	CDebug cDbg("IniRead");

	char *pFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	char lpszBuf[MAX_PATH];
	sprintf(lpszBuf, "%s\\%s", Vars.szScriptPath, pFileName);

    char   szBuffer[65535];        // Max ini line length is 65535 under 95

    char   *pSectionName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    char   *pKeyName = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
    char   *pDefault = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));

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
	CDebug cDbg("IniWrite");

	char *pFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	char lpszBuf[MAX_PATH];
	sprintf(lpszBuf, "%s\\%s", Vars.szScriptPath, pFileName);

    char   *pSectionName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
    char   *pKeyName = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
    char   *pValue = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
	if(WritePrivateProfileString(pSectionName, pKeyName, pValue, lpszBuf))
		WritePrivateProfileString(NULL, NULL, NULL, lpszBuf);	// Flush

	return JS_TRUE;
}

JSAPI_FUNC(my_login)
{
	CDebug cDbg("login");

	char file[_MAX_FNAME+MAX_PATH], *profile,
		 mode[15], username[48], password[256], gateway[256], charname[24],
		 maxLoginTime[10], maxCharTime[10];

	bool handledCase = false;
	int loginTime = 0, charTime = 0;

	if(!JSVAL_IS_STRING(argv[0]))
		THROW_ERROR(cx, obj, "Invalid profile specified!");

	profile = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));

	sprintf(file, "%sd2bs.ini", Vars.szPath);
	GetPrivateProfileString(profile, "mode", "single", mode, sizeof(mode), file);
	GetPrivateProfileString(profile, "character", "ERROR", charname, sizeof(charname), file);

	// Look for the version string, otherwise return.
	if(!findControl(4,0,599,200,40))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	Vars.bBlockKeys = Vars.bBlockMouse = 1;

	switch(tolower(mode[0]))
	{
		case 't':
			// TCP/IP login - Not implemented.
			GetPrivateProfileString(profile, "gateway", "ERROR", gateway, sizeof(gateway), file);
			GetPrivateProfileString("settings", "MaxLoginTime", "-1", maxLoginTime, sizeof(maxLoginTime), file);
			GetPrivateProfileString("settings", "MaxCharSelectTime", "-1", maxCharTime, sizeof(maxCharTime), file);

			loginTime = atoi(maxLoginTime);
			charTime = atoi(maxCharTime);

			*rval = JSVAL_FALSE;
			Vars.bBlockKeys = Vars.bBlockMouse = 0;
			return JS_TRUE;
			break;
		case 's':
			// Single player button
			if(!clickControl(findControl(6,264,324,272,35)))
				THROW_ERROR(cx, obj, "Failed to click the 'single player' button?");
			break;
		case 'b':
			// Battle.net login
			if(!clickControl(findControl(6,264,366,272,35)))
				THROW_ERROR(cx, obj, "Failed to click the 'Battle.net' button?");
			handledCase = true;
			
		case 'o':
			// Open Battle.net login
			GetPrivateProfileString(profile, "username", "ERROR", username, sizeof(username), file);
			GetPrivateProfileString(profile, "password", "ERROR", password, sizeof(password), file);
			GetPrivateProfileString(profile, "gateway", "ERROR", gateway, sizeof(gateway), file);
			GetPrivateProfileString("settings", "MaxLoginTime", "-1", maxLoginTime, sizeof(maxLoginTime), file);
			GetPrivateProfileString("settings", "MaxCharSelectTime", "-1", maxCharTime, sizeof(maxCharTime), file);

			loginTime = atoi(maxLoginTime);
			charTime = atoi(maxCharTime);

			// handle above fall-through case because open and closed use the same method, just a different button set
			if(!handledCase)
			{
				// Other Multiplayer
				if(!clickControl(findControl(6,264,433,272,35)))
					THROW_ERROR(cx, obj, "Failed to click the 'Other Multiplayer' button?");
				// Open Battle.net
				if(!clickControl(findControl(6,264,310,272,35)))
					THROW_ERROR(cx, obj, "Failed to click the 'Open Battle.net' button?");
			}

			// Connecting
			for(int i = 0; findControl(4,222,360,340,70); i++)
			{
				Sleep(500);
				if(i*500 > loginTime)
					THROW_ERROR(cx, obj, "Exceeded max login time");
			}

			// handle case where cd key is banned, etc.

			// Username text-edit box
			SetControlText(findControl(1,322,342,162,19), username);
			// Password text-edit box
			SetControlText(findControl(1,322,396,162,19), password);
			// Log-in
			if(!clickControl(findControl(6,264,484,272,35)))
				THROW_ERROR(cx, obj, "Failed to click the 'Log in' button?");
			// Connecting
			for(int i = 0; findControl(6,351,337,96,32); i++)
			{
				Sleep(500);
				if(i*500 > charTime)
					THROW_ERROR(cx, obj, "Exceeded max character select time");
			}

			break;
		default:
			THROW_ERROR(cx, obj, "Invalid login mode specified!");
			break;
	}

	*rval = BOOLEAN_TO_JSVAL(OOG_SelectCharacter(charname));

	Vars.bBlockKeys = Vars.bBlockMouse = 0;

	return JS_TRUE;
}

JSAPI_FUNC(my_getMouseCoords)
{
	CDebug cDbg("getMouseCoords");

	jsint nFlag = 0;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nFlag = JSVAL_TO_INT(argv[0]);

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);

	if(!pReturnArray)
		return JS_TRUE;

	POINT Coords = {*p_D2CLIENT_MouseX, *p_D2CLIENT_MouseY};

	if(nFlag == 1)
	{
		Coords.x += *p_D2CLIENT_MouseOffsetX;
		Coords.y += *p_D2CLIENT_MouseOffsetY;

		D2COMMON_AbsScreenToMap(&Coords.x, &Coords.y);
	}

	jsval jsX = INT_TO_JSVAL(Coords.x);
	jsval jsY = INT_TO_JSVAL(Coords.y);

	JS_SetElement(cx, pReturnArray, 0, &jsX);
	JS_SetElement(cx, pReturnArray, 1, &jsY);

	*rval = OBJECT_TO_JSVAL(pReturnArray);
	
	return JS_TRUE;
}

JSAPI_FUNC(my_submitItem)
{
	CDebug cDbg("submitItem");

	if(UnitAny* pUnit = D2CLIENT_GetCursorItem())
	{
		D2CLIENT_submitItem(pUnit->dwUnitId);
		*rval = BOOLEAN_TO_JSVAL(TRUE);
	} else *rval = BOOLEAN_TO_JSVAL(FALSE);

	return JS_TRUE;
}

JSAPI_FUNC(my_getInteractedNPC)
{
	UnitAny* pNPC = D2CLIENT_GetCurrentInteractingNPC();
	if (!pNPC) {
		*rval = BOOLEAN_TO_JSVAL(FALSE);
		return JS_TRUE;
	}

	myUnit* pmyUnit = new myUnit; // leaked?

	if(!pmyUnit)
		return JS_TRUE;

	CHAR szName[128] = "";
	pmyUnit->_dwPrivateType = PRIVATE_UNIT;
	pmyUnit->dwClassId = pNPC->dwTxtFileNo;
	pmyUnit->dwMode = pNPC->dwMode;
	pmyUnit->dwType = pNPC->dwType;
	pmyUnit->dwUnitId = pNPC->dwUnitId;
	strcpy(pmyUnit->szName, szName);

	JSObject *jsunit = BuildObject(cx, &unit_class, unit_methods, unit_props, pmyUnit);

	if(!jsunit)
		return JS_TRUE;

	*rval = OBJECT_TO_JSVAL(jsunit);
	return JS_TRUE;
}