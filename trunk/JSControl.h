#pragma once

#include "js32.h"
#include <windows.h>
#include "D2Ptrs.h"

CLASS_CTOR(control);

void control_finalize(JSContext *cx, JSObject *obj);

JSAPI_FUNC(control_getNext);
JSAPI_FUNC(control_click);
JSAPI_FUNC(control_setText);
JSAPI_FUNC(control_getText);

JSAPI_PROP(control_x);
JSAPI_PROP(control_y);
JSAPI_PROP(control_xsize);
JSAPI_PROP(control_ysize);
JSAPI_PROP(control_pwd);
JSAPI_PROP(control_maxlen);
JSAPI_PROP(control_type);
JSAPI_PROP(control_visible);
JSAPI_PROP(control_selstart);
JSAPI_PROP(control_selend);

JSAPI_PROP(control_get_text);
JSAPI_PROP(control_get_state);
JSAPI_PROP(control_get_curpos);
JSAPI_PROP(control_set_text);
JSAPI_PROP(control_set_state);
JSAPI_PROP(control_set_curpos);

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
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
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
	{"text",		CONTROL_TEXT,			JSPROP_STATIC_VAR,		control_get_text,	control_set_text},
	{"x",			CONTROL_X,				JSPROP_PERMANENT_VAR,	control_x},
	{"y",			CONTROL_Y,				JSPROP_PERMANENT_VAR,	control_y},
	{"xsize",		CONTROL_XSIZE,			JSPROP_PERMANENT_VAR,	control_xsize},
	{"ysize",		CONTROL_YSIZE,			JSPROP_PERMANENT_VAR,	control_ysize},
	{"state",		CONTROL_STATE,			JSPROP_STATIC_VAR,		control_get_state,	control_set_state},
	{"password",	CONTROL_PASSWORD,		JSPROP_PERMANENT_VAR,	control_pwd},
//	{"maxlength",	CONTROL_MAXLENGTH,		JSPROP_PERMANENT_VAR,	control_maxlen},
	{"type",		CONTROL_TYPE,			JSPROP_PERMANENT_VAR,	control_type},
//	{"visible",		CONTROL_VISIBLE,		JSPROP_PERMANENT_VAR,	control_visible},
	{"cursorpos",	CONTROL_CURSORPOS,		JSPROP_STATIC_VAR,		control_get_curpos,	control_set_curpos},
	{"selectstart",	CONTROL_SELECTSTART,	JSPROP_PERMANENT_VAR,	control_selstart},
	{"selectend",	CONTROL_SELECTEND,		JSPROP_PERMANENT_VAR,	control_selend},
	{0},
};

static JSFunctionSpec control_funcs[] = {
	{"getNext",			control_getNext,		0},
	{"click",			control_click,			0},
	{"setText",			control_setText,		1},
	{"getText",			control_getText,		0},
	{0},
};