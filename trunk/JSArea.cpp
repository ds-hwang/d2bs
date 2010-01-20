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

JSAPI_PROP(area_exits)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
		*vp = INT_TO_JSVAL(pArea->Exits);
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(area_name)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
	{
		Level* pLevel = GetLevel(pArea->AreaId);
		if(!pLevel)
			return JS_FALSE;
		LevelTxt* pTxt = D2COMMON_GetLevelTxt(pArea->AreaId);
		if(pTxt)
			*vp = STRING_TO_JSVAL(JS_InternString(cx, pTxt->szName));
		else
			return JS_FALSE;
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(area_x)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
	{
		Level* pLevel = GetLevel(pArea->AreaId);
		if(!pLevel)
			return JS_FALSE;
		*vp = INT_TO_JSVAL(pLevel->dwPosX);
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(area_y)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
	{
		Level* pLevel = GetLevel(pArea->AreaId);
		if(!pLevel)
			return JS_FALSE;
		*vp = INT_TO_JSVAL(pLevel->dwPosY);
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(area_xsize)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
	{
		Level* pLevel = GetLevel(pArea->AreaId);
		if(!pLevel)
			return JS_FALSE;
		*vp = INT_TO_JSVAL(pLevel->dwSizeX);
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(area_ysize)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
	{
		Level* pLevel = GetLevel(pArea->AreaId);
		if(!pLevel)
			return JS_FALSE;
		*vp = INT_TO_JSVAL(pLevel->dwSizeY);
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(area_id)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(pArea)
	{
		Level* pLevel = GetLevel(pArea->AreaId);
		if(!pLevel)
			return JS_FALSE;
		*vp = INT_TO_JSVAL(pLevel->dwLevelNo);
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}
