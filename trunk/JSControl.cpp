//#include "D2BS.h"
#include "Control.h"
#include "JSControl.h"
#include "CDebug.h"
#include "Helpers.h"

#include "debugnew/debug_new.h"

void control_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("control finalize");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(pData)
	{
//Oh here there
		JS_SetPrivate(cx, obj, NULL);
		delete pData;
	}
}
JSBool control_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("control getProperty");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if(!pControl)
		return JS_TRUE;

	char* tmp = NULL;

	switch(JSVAL_TO_INT(id))
	{
		case CONTROL_TEXT:
			if(pControl->dwIsCloaked == 33)
				break;
			tmp = UnicodeToAnsi((pControl->dwType == 6 ? pControl->wText2 : pControl->wText));
//Oh here there
			*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
			break;
		case CONTROL_X:
			*vp = INT_TO_JSVAL(pControl->dwPosX);
			break;
		case CONTROL_Y:
			*vp = INT_TO_JSVAL(pControl->dwPosY);
			break;
		case CONTROL_XSIZE:
			*vp = INT_TO_JSVAL(pControl->dwSizeX);
			break;
		case CONTROL_YSIZE:
			*vp = INT_TO_JSVAL(pControl->dwSizeY);
			break;
		case CONTROL_STATE:
			*vp = INT_TO_JSVAL(pControl->dwDisabled - 2);
			break;
		case CONTROL_PASSWORD:
			*vp = BOOLEAN_TO_JSVAL(pControl->dwIsCloaked == 33);
			break;
		case CONTROL_TYPE:
			*vp = INT_TO_JSVAL(pControl->dwType);
			break;
		case CONTROL_CURSORPOS:
			*vp = INT_TO_JSVAL(pControl->dwCursorPos);
			break;
		case CONTROL_MAXLENGTH:
//			*vp = INT_TO_JSVAL(pControl->dwMaxLength);
			break;
		case CONTROL_SELECTSTART:
			*vp = INT_TO_JSVAL(pControl->dwSelectStart);
			break;
		case CONTROL_SELECTEND:
			*vp = INT_TO_JSVAL(pControl->dwSelectEnd);
			break;
		default:
			break;
	}

	if(tmp)
		delete[] tmp;

	return JS_TRUE;
}

JSAPI_PROP(control_setProperty) {
	CDebug cDbg("control setProperty");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if(!pControl)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case CONTROL_TEXT: {
			if (!(pControl->dwType == 1))
				return JS_TRUE;
			if(JSVAL_IS_STRING(*vp)) {
//Oh here there
				CHAR* pText	= JS_GetStringBytes(JS_ValueToString(cx, *vp));
				wchar_t* szwText = AnsiToUnicode(pText);
				D2WIN_SetControlText(pControl, szwText);
				delete[] szwText;
			}
			break;
		}
		case CONTROL_STATE: {
			if (JSVAL_IS_INT(*vp)) {
				int nState = JSVAL_TO_INT(*vp);
				if (nState < 0 || nState > 3)
					return JS_TRUE;
				memset((VOID*)&pControl->dwDisabled, (nState + 2), sizeof(DWORD));
			}
			break;
		}
		case CONTROL_CURSORPOS: {
			if (JSVAL_IS_INT(*vp)) {
				DWORD dwPos = JSVAL_TO_INT(*vp);
//				if (dwPos < 0 || dwPos > pControl->dwMaxLength)
//					return JS_TRUE;
				memset((VOID*)&pControl->dwCursorPos, dwPos, sizeof(DWORD));
			}
			break;
		}
	}
	return JS_TRUE;
}

JSBool control_getNext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("control getNext");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if (pControl && pControl->pNext)
		pControl = pControl->pNext;
	else
		pControl = NULL;

	if (!pControl) {
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	if(pControl)
	{
		pData->pControl = pControl;
		pData->dwType = pData->pControl->dwType;
		pData->dwX = pData->pControl->dwPosX;
		pData->dwY = pData->pControl->dwPosY;
		pData->dwSizeX = pData->pControl->dwSizeX;
		pData->dwSizeY = pData->pControl->dwSizeY;
//Oh here there
		JS_SetPrivate(cx, obj, pData);
		*rval = OBJECT_TO_JSVAL(obj);
	}
	else
	{
//Oh here there
		JS_ClearScope(cx, obj);
//Oh here there
		JS_ValueToObject(cx, JSVAL_NULL, &obj);
		*rval = INT_TO_JSVAL(0);
	}
	
	return JS_TRUE;
}

JSBool control_click(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("Control click");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if (!pControl) {
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	jsint x=-1,y=-1;

	if(argc > 1 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
	{
		x = JSVAL_TO_INT(argv[0]);
		y = JSVAL_TO_INT(argv[1]);
	}

	clickControl(pControl, x, y);

	return JS_TRUE;
}

JSBool control_setText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("Control setText");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if (!pControl) {
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}
	
	if(argc < 0 || !JSVAL_IS_STRING(argv[0]))
		return JS_TRUE;

//Oh here there
	char* pText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	wchar_t* szwText = AnsiToUnicode(pText);

	D2WIN_SetControlText(pControl, szwText);

	delete[] szwText;
	return JS_TRUE;
}

JSBool control_getText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("Control getText");

//Oh here there
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if (!pControl) {
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	if(pControl->dwType != 4 || !pControl->pFirstText)
		return JS_TRUE;

//Oh here there
	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
	INT nArrayCount = 0;

	for(ControlText* pText = pControl->pFirstText; pText; pText = pText->pNext)
	{
		if(!pText->wText)
			continue;

		char* tmp = UnicodeToAnsi(pText->wText);

//Oh here there
		jsval aString = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, tmp));
		delete[] tmp;
//Oh here there
		JS_SetElement(cx, pReturnArray, nArrayCount, &aString); 

		nArrayCount++;				
	}

	*rval = OBJECT_TO_JSVAL(pReturnArray);

	return JS_TRUE;
}


INT my_getControl(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("getControl");

	Control* pControl = *p_D2WIN_FirstControl;
	int nType = -1, nX = -1, nY = -1, nXSize = -1, nYSize = -1;
	int *args[] = {&nType, &nX, &nY, &nXSize, &nYSize};

	for(uintN i = 0; i < argc; i++)
		if(JSVAL_IS_INT(argv[i]))
			*args[i] = JSVAL_TO_INT(argv[i]);

	if(argc > 0 && (nType != -1 || nX != -1 || nY != -1 || nXSize != -1 || nYSize != -1)) {
		for(pControl = *p_D2WIN_FirstControl; pControl; pControl = pControl->pNext)
		{
			if((nType != -1 && pControl->dwType == nType) &&
			   (nX != -1 && pControl->dwPosX == nX) &&
			   (nY != -1 && pControl->dwPosY == nY) &&
			   (nXSize != -1 && pControl->dwSizeX == nXSize) &&
			   (nYSize != -1 && pControl->dwSizeY == nYSize))
					break;
		}
	}

	if(!pControl)
		return JS_TRUE;

//Oh here there
	JSObject* control = JS_NewObject(cx, &control_class, NULL, NULL);

//Oh here there
	if(!control || !JS_DefineFunctions(cx, control, control_funcs) || !JS_DefineProperties(cx, control, control_props))
		return JS_TRUE;
	
	ControlData* data = new ControlData;
	data->dwType = nType;
	data->dwX = nX;
	data->dwY = nY;
	data->dwSizeX = nXSize;
	data->dwSizeY = nYSize;
//Oh here there
	JS_SetPrivate(cx, control, data);

	*rval = OBJECT_TO_JSVAL(control);

	return JS_TRUE;
}