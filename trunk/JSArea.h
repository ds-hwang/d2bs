#ifndef AREA_H
#define AREA_H

#include "js32.h"
#include "CollisionMap.h"

CLASS_CTOR(area);

void area_finalize(JSContext *cx, JSObject *obj);
JSAPI_PROP(area_exits);
JSAPI_PROP(area_name);
JSAPI_PROP(area_x);
JSAPI_PROP(area_y);
JSAPI_PROP(area_xsize);
JSAPI_PROP(area_ysize);
JSAPI_PROP(area_id);

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

static JSClass area_class = {
    "Area",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, area_finalize,
    NULL, NULL, NULL, area_ctor
};

enum area_tinyid {
	AUNIT_EXITS,			// 1
	AUNIT_NAME,				// 2
	AUNIT_X,				// 3
	AUNIT_XSIZE,			// 4
	AUNIT_Y,				// 5
	AUNIT_YSIZE,			// 6
	AUNIT_ID,
};


static JSPropertySpec area_props[] = {
	{"exits",		AUNIT_EXITS,		JSPROP_PERMANENT_VAR,	area_exits},
	{"name",		AUNIT_NAME,			JSPROP_PERMANENT_VAR,	area_name},
	{"x",			AUNIT_X,			JSPROP_PERMANENT_VAR,	area_x},
	{"xsize",		AUNIT_XSIZE,		JSPROP_PERMANENT_VAR,	area_xsize},
	{"y",			AUNIT_Y,			JSPROP_PERMANENT_VAR,	area_y},
	{"ysize",		AUNIT_YSIZE,		JSPROP_PERMANENT_VAR,	area_ysize},
	{"id",			AUNIT_ID,			JSPROP_PERMANENT_VAR,	area_id},
	{0},
};

struct myArea {
	DWORD AreaId;
	DWORD Exits;
	LPLevelExit ExitArray[255];
};

#endif