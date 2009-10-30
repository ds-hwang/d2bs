#pragma once

#include "js32.h"

JSAPI_PROP(script_getProperty);

JSAPI_FUNC(script_getNext);
JSAPI_FUNC(script_stop);
JSAPI_FUNC(script_send);
JSAPI_FUNC(script_pause);
JSAPI_FUNC(script_resume);

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/


static JSClass script_class = {
    "D2BSScript",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, script_getProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
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
	{"pause",			script_pause,		0},
	{"resume",			script_resume,		0},
	{"stop",			script_stop,		0},
	{"send",			script_send,		1},
	{0},
};
