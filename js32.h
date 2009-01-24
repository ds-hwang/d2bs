#pragma once

#define JS_THREADSAFE
#define XP_WIN

#include "js32/jsapi.h"

#define JSAPI_FUNC(fName) JSBool fName (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
#define JSAPI_PROP(fName) JSBool fName (JSContext *cx, JSObject *obj, jsval id, jsval *vp)
#define JSPROP_CONSTANT (JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT)
#define JSPROP_PERMANENT_VAR (JSPROP_READONLY | JSPROP_ENUMERATE | JSPROP_PERMANENT)
#define JSPROP_STATIC_VAR (JSPROP_ENUMERATE | JSPROP_PERMANENT)

#define JS_AddRoot(cx, vp) JS_AddNamedRoot((cx), (vp), #vp)

JSBool ThrowJSError(JSContext* cx, JSObject* obj, const char* format, ...);
JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv = NULL, JSObject* proto = NULL, JSObject* parent = NULL);
#define THROW_ERROR(cx, obj, msg) return ThrowJSError(cx, obj, msg)
