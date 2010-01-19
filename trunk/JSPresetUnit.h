#ifndef PRESETUNIT_H
#define PRESETUNIT_H

#include <windows.h>
#include "js32.h"

CLASS_CTOR(presetunit);

void presetunit_finalize(JSContext *cx, JSObject *obj);
JSAPI_PROP(presetunit_getProperty);


/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

static JSClass presetunit_class = {
    "PresetUnit",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, presetunit_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, presetunit_finalize,
    NULL, NULL, NULL, presetunit_ctor
};

enum presetunit_tinyid {
	PUNIT_TYPE,			// 0
	PUNIT_ROOMX,		// 1
	PUNIT_ROOMY,		// 2
	PUNIT_X,			// 3
	PUNIT_Y,			// 4
	PUNIT_ID,			// 5
	PUINT_LEVEL
};


static JSPropertySpec presetunit_props[] = {
	{"type",		PUNIT_TYPE,			JSPROP_PERMANENT_VAR},
	{"roomx",		PUNIT_ROOMX,		JSPROP_PERMANENT_VAR},
	{"roomy",		PUNIT_ROOMY,		JSPROP_PERMANENT_VAR},
	{"x",			PUNIT_X,			JSPROP_PERMANENT_VAR},
	{"y",			PUNIT_Y,			JSPROP_PERMANENT_VAR},
	{"id",			PUNIT_ID,			JSPROP_PERMANENT_VAR},
	{"level",		PUINT_LEVEL,		JSPROP_PERMANENT_VAR},
	{0},
};

struct myPresetUnit
{
	DWORD dwType;
	DWORD dwRoomX;
	DWORD dwRoomY;
	DWORD dwPosX;
	DWORD dwPosY;
	DWORD dwId;
	DWORD dwLevel;
};

#endif