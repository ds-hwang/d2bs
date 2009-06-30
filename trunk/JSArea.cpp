//#include "D2BS.h"
#include "JSArea.h"
#include "D2Ptrs.h"
#include "CDebug.h"

#include "debugnew/debug_new.h"

VOID area_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("area finalize");

//Oh here there
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(pArea)
	{
//Oh here there
		JS_SetPrivate(cx, obj, NULL);
		delete pArea;
	}
}

INT area_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("area getProperty");
	
	*vp = INT_TO_JSVAL(0);
//Oh here there
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(!pArea)
		return JS_TRUE;

	Level* pLevel = GetLevel(pArea->AreaId);
	//char* tmp = NULL;

	if(!pLevel)
		return JS_TRUE;
	
	switch(JSVAL_TO_INT(id))
	{
		case AUNIT_EXITS:
			*vp = INT_TO_JSVAL(pArea->Exits);
			break;
		case AUNIT_NAME:
			LevelTxt* pTxt;
			pTxt = D2COMMON_GetLevelTxt(pArea->AreaId);
			if(!pTxt)
				break;
//Oh here there
			*vp = STRING_TO_JSVAL(JS_InternString(cx, pTxt->szName));
			break;
		case AUNIT_X:
			*vp = INT_TO_JSVAL(pLevel->dwPosX);
			break;
		case AUNIT_Y:
			*vp = INT_TO_JSVAL(pLevel->dwPosY);
			break;
		case AUNIT_XSIZE:
			*vp = INT_TO_JSVAL(pLevel->dwSizeX);
			break;
		case AUNIT_YSIZE:
			*vp = INT_TO_JSVAL(pLevel->dwSizeY);
			break;
		case AUNIT_ID:
			*vp = INT_TO_JSVAL(pLevel->dwLevelNo);
			break;
		default:
			break;
	}
	return JS_TRUE;
}