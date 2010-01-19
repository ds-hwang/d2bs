#pragma once

#include "js32.h"
#include <windows.h>
#include "D2Ptrs.h"

CLASS_CTOR(control);

void control_finalize(JSContext *cx, JSObject *obj);
JSAPI_PROP(control_getProperty);
JSAPI_PROP(control_setProperty);

JSAPI_FUNC(control_getNext);
JSAPI_FUNC(control_click);
JSAPI_FUNC(control_setText);
JSAPI_FUNC(control_getText);

struct ControlData {
	DWORD _dwPrivate;
	Control* pControl;

	DWORD	dwType;
	DWORD	dwX;
	DWORD	dwY;
	DWORD	dwSizeX;
	DWORD	dwSizeY;
};

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

static JSClass control_class = {
    "Control",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, control_getProperty, control_setProperty,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, control_finalize,
    NULL, NULL, NULL, control_ctor
};

enum control_tinyid {
	CONTROL_TEXT = -1,
	CONTROL_X = -2,
	CONTROL_Y = -3,
	CONTROL_XSIZE = -4,
	CONTROL_YSIZE = -5,
	CONTROL_STATE = -6,
	CONTROL_MAXLENGTH = -7,
	CONTROL_TYPE = -8,
	CONTROL_VISIBLE = -9,
	CONTROL_CURSORPOS = -10,
	CONTROL_SELECTSTART = -11,
	CONTROL_SELECTEND = -12,
	CONTROL_PASSWORD = -13,
};


static JSPropertySpec control_props[] = {
	{"text",		CONTROL_TEXT,			JSPROP_STATIC_VAR},
	{"x",			CONTROL_X,				JSPROP_PERMANENT_VAR},
	{"y",			CONTROL_Y,				JSPROP_PERMANENT_VAR},
	{"xsize",		CONTROL_XSIZE,			JSPROP_PERMANENT_VAR},
	{"ysize",		CONTROL_YSIZE,			JSPROP_PERMANENT_VAR},
	{"state",		CONTROL_STATE,			JSPROP_STATIC_VAR},
	{"password",	CONTROL_PASSWORD,		JSPROP_PERMANENT_VAR},
	{"maxlength",	CONTROL_MAXLENGTH,		JSPROP_PERMANENT_VAR},
	{"type",		CONTROL_TYPE,			JSPROP_PERMANENT_VAR},
	{"visible",		CONTROL_VISIBLE,		JSPROP_PERMANENT_VAR},
	{"cursorpos",	CONTROL_CURSORPOS,		JSPROP_STATIC_VAR},
	{"selectstart",	CONTROL_SELECTSTART,	JSPROP_PERMANENT_VAR},
	{"selectend",	CONTROL_SELECTEND,		JSPROP_PERMANENT_VAR},
	{0},
};

static JSFunctionSpec control_funcs[] = {
	{"getNext",			control_getNext,		0},
	{"click",			control_click,			0},
	{"setText",			control_setText,		1},
	{"getText",			control_getText,		0},
	{0},
};