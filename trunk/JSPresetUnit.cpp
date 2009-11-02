#include "JSPresetUnit.h"

#include "debugnew/debug_new.h"

VOID presetunit_finalize(JSContext *cx, JSObject *obj)
{
	myPresetUnit *pUnit = (myPresetUnit*)JS_GetPrivate(cx, obj);

	if(pUnit)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete pUnit;
	}
}

INT presetunit_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	myPresetUnit* pUnit = (myPresetUnit*)JS_GetPrivate(cx, obj);

	if(!pUnit)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case PUNIT_TYPE:
			*vp = INT_TO_JSVAL(pUnit->dwType);
			break;
		case PUNIT_ROOMX:
			*vp = INT_TO_JSVAL(pUnit->dwRoomX);
			break;
		case PUNIT_ROOMY:
			*vp = INT_TO_JSVAL(pUnit->dwRoomY);
			break;
		case PUNIT_X:
			*vp = INT_TO_JSVAL(pUnit->dwPosX);
			break;
		case PUNIT_Y:
			*vp = INT_TO_JSVAL(pUnit->dwPosY);
			break;
		case PUNIT_ID:
			*vp = INT_TO_JSVAL(pUnit->dwId);
			break;
		default:
			break;
	}	
	return JS_TRUE;
}
