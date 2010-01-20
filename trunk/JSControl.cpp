#include "Control.h"
#include "JSControl.h"
#include "Helpers.h"
#include "D2Helpers.h"

EMPTY_CTOR(control)

void control_finalize(JSContext *cx, JSObject *obj)
{
	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));

	if(pData)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete pData;
	}
}

Control* CheckAndFind(JSContext* cx, JSObject* obj)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));
	if(!pData)
		return NULL;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
		return NULL;
	return pControl;
}

JSAPI_PROP(control_x)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwPosX, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_y)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwPosY, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_xsize)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwSizeX, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_ysize)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwSizeY, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_pwd)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	*vp = BOOLEAN_TO_JSVAL(ctrl->dwIsCloaked == 33);
	return JS_TRUE;
}

JSAPI_PROP(control_maxlen)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
//	JS_NewNumberValue(cx, ctrl->dwMaxLength, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_type)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwType, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_visible)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;

	return JS_TRUE;
}

JSAPI_PROP(control_selstart)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwSelectStart, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_selend)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwSelectEnd, vp);
	return JS_TRUE;
}


JSAPI_PROP(control_get_text)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	if(ctrl->dwIsCloaked != 33)
	{
		char* tmp = UnicodeToAnsi((ctrl->dwType == 6 ? ctrl->wText2 : ctrl->wText));
		*vp = STRING_TO_JSVAL(JS_InternString(cx, tmp));
		delete[] tmp;
	}
	return JS_TRUE;
}

JSAPI_PROP(control_get_state)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, (ctrl->dwDisabled - 2), vp);
	return JS_TRUE;
}

JSAPI_PROP(control_get_curpos)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	JS_NewNumberValue(cx, ctrl->dwCursorPos, vp);
	return JS_TRUE;
}

JSAPI_PROP(control_set_text)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(ctrl && ctrl->dwType == 1 && JSVAL_IS_STRING(*vp))
	{
		char* pText = JS_GetStringBytes(JS_ValueToString(cx, *vp));
		if(!pText)
			return JS_TRUE;
		wchar_t* szwText = AnsiToUnicode(pText);
		D2WIN_SetControlText(ctrl, szwText);
		delete[] szwText;
	}
	else
		return JS_FALSE;
	return JS_TRUE;
}

JSAPI_PROP(control_set_state)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	if(JSVAL_IS_INT(*vp))
	{
		int32 nState;
		if(!JS_ValueToECMAInt32(cx, *vp, &nState) || nState < 0 || nState > 3)
			THROW_ERROR(cx, obj, "Invalid state value");
		memset((VOID*)&ctrl->dwDisabled, (nState + 2), sizeof(DWORD));
	}
	return JS_TRUE;
}

JSAPI_PROP(control_set_curpos)
{
	Control* ctrl = CheckAndFind(cx, obj);
	if(!ctrl)
		return JS_FALSE;
	if(JSVAL_IS_INT(*vp))
	{
		DWORD dwPos;
		if(!JS_ValueToECMAUint32(cx, *vp, &dwPos))
			THROW_ERROR(cx, obj, "Invalid cursor position value");
		memset((VOID*)&ctrl->dwCursorPos, dwPos, sizeof(DWORD));
	}
	return JS_TRUE;
}

JSAPI_FUNC(control_getNext)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(pControl && pControl->pNext)
		pControl = pControl->pNext;
	else
		pControl = NULL;

	if(!pControl)
	{
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
		JS_SetPrivate(cx, obj, pData);
		*rval = OBJECT_TO_JSVAL(obj);
	}
	else
	{
		JS_ClearScope(cx, obj);
		if(JS_ValueToObject(cx, JSVAL_NULL, &obj) == JS_FALSE)
			return JS_TRUE;
		*rval = INT_TO_JSVAL(0);
	}
	
	return JS_TRUE;
}

JSAPI_FUNC(control_click)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
	{
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}

	uint32 x = (uint32)-1, y = (uint32)-1;

	if(argc > 1 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
	{
		JS_ValueToECMAUint32(cx, argv[0], &x);
		JS_ValueToECMAUint32(cx, argv[1], &y);
	}

	clickControl(pControl, x, y);

	return JS_TRUE;
}

JSAPI_FUNC(control_setText)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
	{
		*rval = INT_TO_JSVAL(0);
		return JS_TRUE;
	}
	
	if(argc < 0 || !JSVAL_IS_STRING(argv[0]))
		return JS_TRUE;

	char* pText = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!pText)
		return JS_TRUE;
	wchar_t* szwText = AnsiToUnicode(pText);

	D2WIN_SetControlText(pControl, szwText);

	delete[] szwText;
	return JS_TRUE;
}

JSAPI_FUNC(control_getText)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	ControlData *pData = ((ControlData*)JS_GetPrivate(cx, obj));
	if(!pData)
		return JS_TRUE;

	Control* pControl = findControl(pData->dwType, (char *)NULL, -1, pData->dwX, pData->dwY, pData->dwSizeX, pData->dwSizeY);
	if(!pControl)
	{
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


JSAPI_FUNC(my_getControl)
{
	if(ClientState() != ClientStateMenu)
		return JS_TRUE;

	int nType = -1, nX = -1, nY = -1, nXSize = -1, nYSize = -1;
	int *args[] = {&nType, &nX, &nY, &nXSize, &nYSize};

	for(uintN i = 0; i < argc; i++)
		if(JSVAL_IS_INT(argv[i]))
			*args[i] = JSVAL_TO_INT(argv[i]);

	Control* pControl = findControl(nType, (char*)NULL, -1, nX, nY, nXSize, nYSize);
	if(!pControl)
		return JS_TRUE;

	ControlData* data = new ControlData;
	data->dwType = nType;
	data->dwX = nX;
	data->dwY = nY;
	data->dwSizeX = nXSize;
	data->dwSizeY = nYSize;

	JSObject* control = BuildObject(cx, &control_class, control_funcs, control_props, data);
	if(!control)
		THROW_ERROR(cx, obj, "Failed to build control!");

	*rval = OBJECT_TO_JSVAL(control);

	return JS_TRUE;
}

