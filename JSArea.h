#ifndef AREA_H
#define AREA_H


VOID area_finalize(JSContext *cx, JSObject *obj);
INT area_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

static JSClass area_class = {
    "Area",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, area_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, area_finalize,
    NULL, NULL, 
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
	{"exits",		AUNIT_EXITS,		JSPROP_PERMANENT_VAR},
	{"name",		AUNIT_NAME,			JSPROP_PERMANENT_VAR},
	{"x",			AUNIT_X,			JSPROP_PERMANENT_VAR},
	{"xsize",		AUNIT_XSIZE,		JSPROP_PERMANENT_VAR},
	{"y",			AUNIT_Y,			JSPROP_PERMANENT_VAR},
	{"ysize",		AUNIT_YSIZE,		JSPROP_PERMANENT_VAR},
	{"id",			AUNIT_ID,			JSPROP_PERMANENT_VAR},
	{0},
};

struct myArea {
	DWORD AreaId;
	DWORD Exits;
	LPLevelExit ExitArray[255];
};

#endif