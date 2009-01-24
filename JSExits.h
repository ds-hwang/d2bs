#ifndef EXITS_H
#define EXITS_H

VOID exit_finalize(JSContext *cx, JSObject *obj);
INT exit_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);


/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

static JSClass exit_class = {
    "Exit",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, exit_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, exit_finalize,
    NULL, NULL, 
};

enum exit_tinyid {
	EXIT_X,
	EXIT_Y,
	EXIT_ID,
	EXIT_TYPE,
};


static JSPropertySpec exit_props[] = {
	{"x",			EXIT_X,			JSPROP_PERMANENT_VAR},
	{"y",			EXIT_Y,			JSPROP_PERMANENT_VAR},
	{"id",			EXIT_ID,		JSPROP_PERMANENT_VAR},
	{"type",		EXIT_TYPE,		JSPROP_PERMANENT_VAR},
	{0},
};

struct myExit {
	DWORD x;
	DWORD y;
	DWORD id;
	DWORD type;
};

#endif