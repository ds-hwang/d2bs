#pragma once

void control_finalize(JSContext *cx, JSObject *obj);
JSAPI_PROP(control_getProperty);

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
    JS_PropertyStub, JS_PropertyStub, control_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, control_finalize,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

enum control_tinyid {
	CONTROL_TEXT = -1,
	CONTROL_X = -2,
	CONTROL_Y = -3,
	CONTROL_XSIZE = -4,
	CONTROL_YSIZE = -5,
	CONTROL_DISABLED = -6,
	CONTROL_MAXLENGTH = -7,
	CONTROL_TYPE = -8,
	CONTROL_VISIBLE = -9,
};


static JSPropertySpec control_props[] = {
	{"text",		CONTROL_TEXT,			JSPROP_PERMANENT_VAR},
	{"x",			CONTROL_X,				JSPROP_PERMANENT_VAR},
	{"y",			CONTROL_Y,				JSPROP_PERMANENT_VAR},
	{"xsize",		CONTROL_XSIZE,			JSPROP_PERMANENT_VAR},
	{"ysize",		CONTROL_YSIZE,			JSPROP_PERMANENT_VAR},
	{"disabled",	CONTROL_DISABLED,		JSPROP_PERMANENT_VAR},
	{"maxlength",	CONTROL_MAXLENGTH,		JSPROP_PERMANENT_VAR},
	{"type",		CONTROL_TYPE,			JSPROP_PERMANENT_VAR},
	{"visible",		CONTROL_VISIBLE,		JSPROP_PERMANENT_VAR},
	{0},
};

static JSFunctionSpec control_funcs[] = {
	{"getNext",			control_getNext,		0},
	{"click",			control_click,			0},
	{"setText",			control_setText,		1},
	{"getText",			control_getText,		0},
	{0},
};