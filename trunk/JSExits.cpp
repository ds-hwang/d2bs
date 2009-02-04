#include "D2BS.h"
#include "JSExits.h"

#include "debugnew/debug_new.h"

VOID exit_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("exit finalize");

	myExit* pExit = (myExit*)JS_GetPrivate(cx, obj);
	if(pExit)
		delete pExit;
}

INT exit_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("exit getProperty");

	myExit* pExit = (myExit*)JS_GetPrivate(cx, obj);

	*vp = BOOLEAN_TO_JSVAL(FALSE);

	if(!pExit)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case EXIT_X:
			*vp = INT_TO_JSVAL(pExit->x);
			break;
		case EXIT_Y:
			*vp = INT_TO_JSVAL(pExit->y);
			break;
		case EXIT_ID:
			*vp = INT_TO_JSVAL(pExit->id);
			break;
		case EXIT_TYPE:
			*vp = INT_TO_JSVAL(pExit->type);
			break;
		case EXIT_TILEID:
			*vp = INT_TO_JSVAL(pExit->tileid);
			break;
		default:
			break;
	}

	return JS_TRUE;
}
