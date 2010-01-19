#include "JSArea.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"

EMPTY_CTOR(area)

void area_finalize(JSContext *cx, JSObject *obj)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(pArea)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete pArea;
	}
}

JSAPI_PROP(area_getProperty)
{
	*vp = INT_TO_JSVAL(0);
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(!pArea)
		return JS_TRUE;

	Level* pLevel = GetLevel(pArea->AreaId);
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

