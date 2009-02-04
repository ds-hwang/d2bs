#ifndef JSSANDBOX_H
#define JSSANDBOX_H

#include "Script.h"

JSBool sandbox_ctor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

#define DEFPROP(x) JSBool x (JSContext *cx, JSObject *obj, jsval id, jsval *vp);

DEFPROP(sandbox_addProperty)
DEFPROP(sandbox_delProperty)
DEFPROP(sandbox_getProperty)
DEFPROP(sandbox_setProperty)

#undef DEFPROP

#define DEFFUNC(x) JSBool x (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

DEFFUNC(sandbox_eval)
DEFFUNC(sandbox_include)
DEFFUNC(sandbox_isIncluded)
DEFFUNC(sandbox_clear)

#undef DEFFUNC

void sandbox_finalize(JSContext *cx, JSObject *obj);

struct sandbox {
	JSContext* context;
	JSObject* innerObj;
	IncludeList list;
};

static JSClass sandbox_class = {
	"Sandbox",
	JSCLASS_HAS_PRIVATE,
	sandbox_addProperty, sandbox_delProperty, sandbox_getProperty, sandbox_setProperty,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, sandbox_finalize,
	NULL, NULL, NULL, sandbox_ctor, NULL, NULL, NULL, NULL
};

static JSFunctionSpec sandbox_methods[] = {
	{"evaluate",	sandbox_eval,		1},
	{"include",		sandbox_include,	1},
	{"isIncluded",	sandbox_isIncluded,	1},
	{"clearScope",	sandbox_clear,		0},
	{0}
};

#endif
