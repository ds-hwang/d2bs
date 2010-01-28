#include "D2Ptrs.h"
#include "JSDraw.h"
#include "D2Helpers.h"
#include "D2BS.h"

JSAPI_FUNC(drawFrame)
{
	if(argc != 4 || !(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) &&
			JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3])))
		THROW_ERROR(cx, obj, "Invalid arguments passed to drawFrame");

	int32 coords[4];
	for(int i = 0; i < 4; i++)
		if(JS_ValueToInt32(cx, argv[i], &coords[i]) == JS_FALSE)
			THROW_ERROR(cx, obj, "Failed to process argument");
	RECT rect = {coords[0], coords[1], coords[2], coords[3]};
	EnterCriticalSection(&Vars.cFrameHookSection);
	D2GFX_DrawFrame(&rect);
	LeaveCriticalSection(&Vars.cFrameHookSection);
	return JS_TRUE;
}

JSAPI_FUNC(drawBox)
{
	if(argc != 6 || !(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) &&
			JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]) &&
			JSVAL_IS_INT(argv[4]) && JSVAL_IS_INT(argv[5])))
		THROW_ERROR(cx, obj, "Invalid arguments passed to drawFrame");

	int32 coords[4], color, opacity;
	for(int i = 0; i < 4; i++)
		if(JS_ValueToInt32(cx, argv[i], &coords[i]) == JS_FALSE)
			THROW_ERROR(cx, obj, "Failed to process argument");
	if(JS_ValueToInt32(cx, argv[4], &color) == JS_FALSE)
		THROW_ERROR(cx, obj, "Failed to process argument");
	if(JS_ValueToInt32(cx, argv[5], &opacity) == JS_FALSE)
		THROW_ERROR(cx, obj, "Failed to process argument");

	RECT rect = {coords[0], coords[1], coords[0]+coords[2], coords[1]+coords[3]};
	EnterCriticalSection(&Vars.cBoxHookSection);
	D2GFX_DrawRectangle(rect.top, rect.left, rect.bottom, rect.right, color, opacity);
	LeaveCriticalSection(&Vars.cBoxHookSection);
	return JS_TRUE;
}

JSAPI_FUNC(drawLine)
{
	if(argc != 5 || !(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]) &&
			JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3])))
		THROW_ERROR(cx, obj, "Invalid arguments passed to drawFrame");

	int32 coords[4], color;
	for(int i = 0; i < 4; i++)
		if(JS_ValueToInt32(cx, argv[i], &coords[i]) == JS_FALSE)
			THROW_ERROR(cx, obj, "Failed to process argument");
	if(JS_ValueToInt32(cx, argv[4], &color) == JS_FALSE)
		THROW_ERROR(cx, obj, "Failed to process argument");

	POINT start = {coords[0], coords[1]}, end = {coords[2], coords[3]};
	EnterCriticalSection(&Vars.cLineHookSection);
	D2GFX_DrawLine(start.x, start.y, end.x, end.y, color, 0xFF);
	LeaveCriticalSection(&Vars.cLineHookSection);
	return JS_TRUE;
}

JSAPI_FUNC(drawText)
{
	if(argc != 5 || !(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_INT(argv[1]) &&
			JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3]) && JSVAL_IS_INT(argv[4])))
		THROW_ERROR(cx, obj, "Invalid arguments passed to drawText");

	char* text = NULL;
	int32 x, y, color, font;

	if(((text = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]))) == NULL) ||
			JS_ValueToInt32(cx, argv[1], &x) == JS_FALSE ||
			JS_ValueToInt32(cx, argv[2], &y) == JS_FALSE ||
			JS_ValueToInt32(cx, argv[3], &color) == JS_FALSE ||
			JS_ValueToInt32(cx, argv[4], &font) == JS_FALSE)
		THROW_ERROR(cx, obj, "Failed to process argument");

	EnterCriticalSection(&Vars.cTextHookSection);
	myDrawText(text, x, y, color, font);
	LeaveCriticalSection(&Vars.cTextHookSection);
	return JS_TRUE;
}

JSAPI_FUNC(drawImage)
{
	if(argc != 4 || !(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_INT(argv[1]) &&
			JSVAL_IS_INT(argv[2]) && JSVAL_IS_INT(argv[3])))
		THROW_ERROR(cx, obj, "Invalid arguments passed to drawText");

	char* path = NULL;
	int32 x, y, color;

	if(((path = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]))) == NULL) ||
			JS_ValueToInt32(cx, argv[1], &x) == JS_FALSE ||
			JS_ValueToInt32(cx, argv[2], &y) == JS_FALSE ||
			JS_ValueToInt32(cx, argv[3], &color) == JS_FALSE)
		THROW_ERROR(cx, obj, "Failed to process argument");

	CellFile* image = LoadCellFile(path);
	EnterCriticalSection(&Vars.cImageHookSection);
	myDrawAutomapCell(image, x, y, (BYTE)color);
	LeaveCriticalSection(&Vars.cImageHookSection);
	return JS_TRUE;
}

JSAPI_FUNC(drawAutomapSymbol)
{
	int x = 0, y = 0, color = 0;

	if(!JS_ConvertArguments(cx, argc, argv, "iii", &x, &y, &color))
		return JS_FALSE;

	EnterCriticalSection(&Vars.cAutomapSymbolSection);
	D2CLIENT_DrawAutomapSymbol(x, y, color);
	LeaveCriticalSection(&Vars.cAutomapSymbolSection);
	return JS_TRUE;
}

bool ConvertArgs(JSContext *cx, uintN &argc, jsval *argv, int32 &x, int32 &y)
{
	if(argc == 1)
	{
		JSObject* arg = NULL;
		if(!JS_ConvertArguments(cx, 1, argv, "o", &arg))
		{
			return false;
		}
		jsval dx = JSVAL_VOID, dy = JSVAL_VOID;
		if(!JS_GetProperty(cx, arg, "x", &dx) || !JS_GetProperty(cx, arg, "y", &dy))
		{
			JS_ReportError(cx, "Argument must have an x and a y property");
			return false;
		}
		if(!JS_ConvertValue(cx, dx, JSTYPE_NUMBER, &(argv[0])) ||
			!JS_ConvertValue(cx, dy, JSTYPE_NUMBER, &(argv[1])) ||
			!JS_ValueToECMAInt32(cx, dx, &x) || !JS_ValueToECMAInt32(cx, dy, &y))
		{
			JS_ReportError(cx, "Failed to convert x or y property");
			return false;
		}
	}
	else if(argc == 2)
	{
		if(!JS_ConvertArguments(cx, 2, argv, "uu", &x, &y))
		{
			JS_ReportError(cx, "Failed to convert x or y property");
			return false;
		}
	}
	return true;
}

JSAPI_FUNC(screenToWorld)
{
	int32 x = 0, y = 0;
	if(!ConvertArgs(cx, argc, argv, x, y))
		return JS_FALSE;

	POINT result = {x, y};
	ScreenToWorld(&result);
	JS_NewNumberValue(cx, result.x, &(argv[0]));
	JS_NewNumberValue(cx, result.y, &(argv[1]));
	JSObject* res = BuildObject(cx);
	if(!JS_SetProperty(cx, res, "x", &argv[0]) || !JS_SetProperty(cx, res, "y", &argv[1]))
	{
		JS_ReportError(cx, "Failed to set x and/or y values");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(res);
	return JS_TRUE;
}

JSAPI_FUNC(worldToScreen)
{
	int32 x = 0, y = 0;
	if(!ConvertArgs(cx, argc, argv, x, y))
		return JS_FALSE;

	POINT result = {x, y};
	WorldToScreen(&result);
	JS_NewNumberValue(cx, result.x, &(argv[0]));
	JS_NewNumberValue(cx, result.y, &(argv[1]));
	JSObject* res = BuildObject(cx);
	if(!JS_SetProperty(cx, res, "x", &argv[0]) || !JS_SetProperty(cx, res, "y", &argv[1]))
	{
		JS_ReportError(cx, "Failed to set x and/or y values");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(res);
	return JS_TRUE;
}

JSAPI_FUNC(screenToAutomap)
{
	int32 x = 0, y = 0;

	if(!ConvertArgs(cx, argc, argv, x, y))
		return JS_FALSE;

	POINT result = {x, y};
	ScreenToAutomap(&result);
	JS_NewNumberValue(cx, result.x, &(argv[0]));
	JS_NewNumberValue(cx, result.y, &(argv[1]));
	JSObject* res = BuildObject(cx);
	if(!JS_SetProperty(cx, res, "x", &argv[0]) || !JS_SetProperty(cx, res, "y", &argv[1]))
	{
		JS_ReportError(cx, "Failed to set x and/or y values");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(res);
	return JS_TRUE;
}

JSAPI_FUNC(automapToScreen)
{
	int32 x = 0, y = 0;

	if(!ConvertArgs(cx, argc, argv, x, y))
		return JS_FALSE;

	// convert the values
	POINT result = {x,y};
	ScreenToAutomap(&result);
	JS_NewNumberValue(cx, result.x, &(argv[0]));
	JS_NewNumberValue(cx, result.y, &(argv[1]));
	JSObject* res = BuildObject(cx, NULL, NULL, NULL);
	if(!JS_SetProperty(cx, res, "x", &argv[0]) || !JS_SetProperty(cx, res, "y", &argv[1]))
	{
		JS_ReportError(cx, "Failed to set x and/or y values");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}

JSAPI_FUNC(worldToAutomap)
{
	int32 x = 0, y = 0;

	if(!ConvertArgs(cx, argc, argv, x, y))
		return JS_FALSE;

	// convert the values
	POINT result = {x,y};
	WorldToScreen(&result);
	ScreenToAutomap(&result);
	JS_NewNumberValue(cx, result.x, &(argv[0]));
	JS_NewNumberValue(cx, result.y, &(argv[1]));
	JSObject* res = BuildObject(cx, NULL, NULL, NULL);
	if(!JS_SetProperty(cx, res, "x", &argv[0]) || !JS_SetProperty(cx, res, "y", &argv[1]))
	{
		JS_ReportError(cx, "Failed to set x and/or y values");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}

JSAPI_FUNC(automapToWorld)
{
	int32 x = 0, y = 0;

	if(!ConvertArgs(cx, argc, argv, x, y))
		return JS_FALSE;

	// convert the values
	POINT result = {x,y};
	AutomapToScreen(&result);
	ScreenToWorld(&result);
	JS_NewNumberValue(cx, result.x, &(argv[0]));
	JS_NewNumberValue(cx, result.y, &(argv[1]));
	JSObject* res = BuildObject(cx, NULL, NULL, NULL);
	if(!JS_SetProperty(cx, res, "x", &argv[0]) || !JS_SetProperty(cx, res, "y", &argv[1]))
	{
		JS_ReportError(cx, "Failed to set x and/or y values");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}
