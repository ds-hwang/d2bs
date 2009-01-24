#pragma once

#include "D2BS.h"
#include "js32.h"
#include "Script.h"

VOID script_finalize(JSContext *cx, JSObject *obj);
INT script_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

#define DEFFNC(f) INT f (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

DEFFNC(script_getNext)
DEFFNC(script_stop)
DEFFNC(script_send)

#undef DEFFNC

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/


static JSClass script_class = {
    "Script",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, script_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, script_finalize,
    NULL, NULL, 
};

enum script_tinyid {
	SCRIPT_FILENAME,
	SCRIPT_GAMETYPE,
	SCRIPT_RUNNING,
	SCRIPT_THREADID,
};


static JSPropertySpec script_props[] = {
	{"name",			SCRIPT_FILENAME,		JSPROP_PERMANENT_VAR},
	{"type",			SCRIPT_GAMETYPE,		JSPROP_PERMANENT_VAR},
	{"running",			SCRIPT_RUNNING,			JSPROP_PERMANENT_VAR},
	{"threadid",		SCRIPT_THREADID,		JSPROP_PERMANENT_VAR},
	{0},
};

static JSFunctionSpec script_methods[] = {
	{"getNext",			script_getNext,		0},
	{"stop",			script_stop,		0},
	{"send",			script_send,		1},
	{0},
};
