#pragma once

#define XP_WIN

#include "jsapi.h"
#define EXPORTING
#include "Exports.hpp"
#undef EXPORTING

struct JSClassSpec {
	JSClass* classp;
	JSClass* proto;
	JSFunctionSpec* methods;
	JSPropertySpec* properties;
	JSFunctionSpec* static_methods;
	JSPropertySpec* static_properties;
};

#define JSCLASS_DEFAULT_STANDARD_MEMBERS \
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, \
    JSCLASS_NO_OPTIONAL_MEMBERS

#define JSPROP_DEFAULT	JSPROP_ENUMERATE|JSPROP_PERMANENT
#define JSPROP_STATIC	JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY

#define JSAPI_FUNC(name) JSBool name##(JSContext *cx, uintN argc, jsval *vp)
#define JSAPI_PROP(name) JSBool name##(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
#define JSAPI_STRICT_PROP(name) JSBool name##(JSContext* cx, JSObject *obj, jsid id, JSBool strict, jsval* vp)

#define THROW_ERROR(cx, obj, message) \
	return JS_ThrowError(cx, obj, message);

EXPORT JSBool JS_ThrowError(JSContext* cx, JSObject* obj, const char* message);
EXPORT void JS_DefineClasses(JSContext* cx, JSObject* obj, JSClassSpec* classes);
