#include "D2BS.h"
#include "JSControl.h"

void control_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("control finalize");

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(pData)
		delete pData;
}
JSBool control_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("control getProperty");

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
			tmp = UnicodeToAnsi(pControl->wText2);
			*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
			if(tmp)
				delete[] tmp;
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
		case CONTROL_DISABLED:
			*vp = INT_TO_JSVAL(pControl->dwIsCloaked);
			break;
		case CONTROL_TYPE:
			*vp = INT_TO_JSVAL(pControl->dwType);
			break;
		default:
			break;
	}

	if(tmp)
		delete[] tmp;

	return JS_TRUE;
}

JSBool control_getNext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("control getNext");

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if (!pControl) {
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	pControl = pControl->pNext;

	if(pControl)
	{
		JS_SetPrivate(cx, obj, pControl);
		*rval = OBJECT_TO_JSVAL(obj);
	}
	else
	{
		JS_ClearScope(cx, obj);
		JS_ValueToObject(cx, JSVAL_NULL, &obj);
		*rval = INT_TO_JSVAL(0);
	}
	
	return JS_TRUE;
}

JSBool control_click(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("Control click");

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(!pData || IsBadReadPtr(pData, sizeof(ControlData)))
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);

	if (!pControl) {
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	jsint x=0,y=0;

	if(argc > 1 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
	{
		x = JSVAL_TO_INT(argv[0]);
		y = JSVAL_TO_INT(argv[1]);
	}
	if(!x && !y)
	{
		x = pControl->dwSizeX / 2;
		y = pControl->dwSizeY / 2;
	}

	SendMouseClick(pControl->dwPosX + x, pControl->dwPosY - y, 0);
	SendMouseClick(pControl->dwPosX + x, pControl->dwPosY - y, 1);

	return JS_TRUE;
}

JSBool control_setText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("Control setText");

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

	char* pText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	wchar_t* szwText = AnsiToUnicode(pText);

	D2WIN_SetControlText(pControl, szwText);

	delete[] szwText;
	return JS_TRUE;
}

JSBool control_getText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("Control getText");

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

	JSObject* pReturnArray = JS_NewArrayObject(cx, 0, NULL);
	INT nArrayCount = 0;

	for(ControlText* pText = pControl->pFirstText; pText; pText = pText->pNext)
	{
		if(!pText->wText)
			continue;

		char* tmp = UnicodeToAnsi(pText->wText);

		jsval aString = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, tmp));
		delete[] tmp;
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

	JSObject* control = JS_NewObject(cx, &control_class, NULL, NULL);

	if(!control || !JS_DefineFunctions(cx, control, control_funcs) || !JS_DefineProperties(cx, control, control_props))
		return JS_TRUE;
	
	ControlData* data = new ControlData;
	data->dwType = nType;
	data->dwX = nX;
	data->dwY = nY;
	data->dwSizeX = nXSize;
	data->dwSizeY = nYSize;
	JS_SetPrivate(cx, control, data);

	*rval = OBJECT_TO_JSVAL(control);

	return JS_TRUE;
}
