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

JSAPI_FUNC(screenToAutomap)
{
	if(argc == 1)
	{
		// the arg must be an object with an x and a y that we can convert
		if(JSVAL_IS_OBJECT(argv[0]))
		{
			// get the params
			JSObject* arg = JSVAL_TO_OBJECT(argv[0]);
			jsval x, y;
			if(JS_GetProperty(cx, arg, "x", &x) == JS_FALSE || JS_GetProperty(cx, arg, "y", &y) == JS_FALSE)
				THROW_ERROR(cx, obj, "Failed to get x and/or y values");
			if(!JSVAL_IS_INT(x) || !JSVAL_IS_INT(y))
				THROW_ERROR(cx, obj, "Input has an x or y, but they aren't the correct type!");
			int32 ix, iy;
			if(JS_ValueToInt32(cx, x, &ix) == JS_FALSE || JS_ValueToInt32(cx, y, &iy))
				THROW_ERROR(cx, obj, "Failed to convert x and/or y values");
			// convert the values
			POINT result;
			ix *= 32;
			iy *= 32;
			ScreenToAutomap(&result, ix, iy);
			x = INT_TO_JSVAL(ix);
			y = INT_TO_JSVAL(iy);
			if(JS_SetProperty(cx, arg, "x", &x) == JS_FALSE || JS_SetProperty(cx, arg, "y", &y) == JS_FALSE)
				THROW_ERROR(cx, obj, "Failed to set x and/or y values");
			*rval = OBJECT_TO_JSVAL(arg);
		}
		else
			THROW_ERROR(cx, obj, "Invalid object specified to screenToAutomap");
	}
	else if(argc == 2)
	{
		// the args must be ints
		if(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
		{
			int32 ix, iy;
			if(JS_ValueToInt32(cx, argv[0], &ix) == JS_FALSE || JS_ValueToInt32(cx, argv[1], &iy))
				THROW_ERROR(cx, obj, "Failed to convert x and/or y values");
			// convert the values
			POINT result;
			ix *= 32;
			iy *= 32;
			ScreenToAutomap(&result, ix, iy);
			argv[0] = INT_TO_JSVAL(ix);
			argv[1] = INT_TO_JSVAL(iy);
			JSObject* res = JS_NewObject(cx, NULL, NULL, NULL);
			if(JS_SetProperty(cx, res, "x", &argv[0]) == JS_FALSE || JS_SetProperty(cx, res, "y", &argv[1]) == JS_FALSE)
				THROW_ERROR(cx, obj, "Failed to set x and/or y values");
			*rval = OBJECT_TO_JSVAL(res);
		}
		else
			THROW_ERROR(cx, obj, "screenToAutomap expects two arguments to be two integers");
	}
	else
		THROW_ERROR(cx, obj, "Invalid arguments specified for screenToAutomap");
	return JS_TRUE;
}

JSAPI_FUNC(automapToScreen)
{
	if(argc == 1)
	{
		// the arg must be an object with an x and a y that we can convert
		if(JSVAL_IS_OBJECT(argv[0]))
		{
			// get the params
			JSObject* arg = JSVAL_TO_OBJECT(argv[0]);
			jsval x, y;
			if(JS_GetProperty(cx, arg, "x", &x) == JS_FALSE || JS_GetProperty(cx, arg, "y", &y) == JS_FALSE)
				THROW_ERROR(cx, obj, "Failed to get x and/or y values");
			if(!JSVAL_IS_INT(x) || !JSVAL_IS_INT(y))
				THROW_ERROR(cx, obj, "Input has an x or y, but they aren't the correct type!");
			int32 ix, iy;
			if(JS_ValueToInt32(cx, x, &ix) == JS_FALSE || JS_ValueToInt32(cx, y, &iy))
				THROW_ERROR(cx, obj, "Failed to convert x and/or y values");
			// convert the values
			POINT result = {ix,iy};
			MapToScreenCoords(&result);
			x = INT_TO_JSVAL(ix);
			y = INT_TO_JSVAL(iy);
			if(JS_SetProperty(cx, arg, "x", &x) == JS_FALSE || JS_SetProperty(cx, arg, "y", &y) == JS_FALSE)
				THROW_ERROR(cx, obj, "Failed to set x and/or y values");
			*rval = OBJECT_TO_JSVAL(arg);
		}
		else
			THROW_ERROR(cx, obj, "Invalid object specified to automapToScreen");
	}
	else if(argc == 2)
	{
		// the args must be ints
		if(JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
		{
			int32 ix, iy;
			if(JS_ValueToInt32(cx, argv[0], &ix) == JS_FALSE || JS_ValueToInt32(cx, argv[1], &iy))
				THROW_ERROR(cx, obj, "Failed to convert x and/or y values");
			// convert the values
			POINT result = {ix,iy};
			MapToScreenCoords(&result);
			argv[0] = INT_TO_JSVAL(result.x);
			argv[1] = INT_TO_JSVAL(result.y);
			JSObject* res = JS_NewObject(cx, NULL, NULL, NULL);
			if(JS_SetProperty(cx, res, "x", &argv[0]) == JS_FALSE || JS_SetProperty(cx, res, "y", &argv[1]) == JS_FALSE)
				THROW_ERROR(cx, obj, "Failed to set x and/or y values");
			*rval = OBJECT_TO_JSVAL(res);
		}
		else
			THROW_ERROR(cx, obj, "automapToScreen expects two arguments to be two integers");
	}
	else
		THROW_ERROR(cx, obj, "Invalid arguments specified for automapToScreen");
	return JS_TRUE;
}