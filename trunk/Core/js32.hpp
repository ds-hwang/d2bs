#pragma once

#define XP_WIN

#include "jsapi.h"
#define EXPORTING
#include "Exports.hpp"
#undef EXPORTING

#include <vector>

// IMPORTANT: Ordering is critical here! If your object has a
// defined prototype, _THAT PROTOTYPE MUST BE LISTED ABOVE IT_
struct JSClassSpec {
	JSClass* classp;
	JSClass* proto;
	JSFunctionSpec* methods;
	JSPropertySpec* properties;
	JSFunctionSpec* static_methods;
	JSPropertySpec* static_properties;
};

struct JSModuleSpec {
	const wchar_t* name;
	JSClassSpec* classes;
	JSFunctionSpec* static_methods;
	JSPropertySpec* static_properties;
};

#define JS_CS(classp, proto, methods, props, static_methods, static_props) \
	{classp, proto, methods, props, static_methods, static_props}

#define JS_CS_END {0}

#define JS_MS(name, classes, static_methods, static_properties) \
	{L##name, classes, static_methods, static_properties}

#define JS_MS_END {0}

#define JSCLASS_DEFAULT_STANDARD_MEMBERS \
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, \
    JSCLASS_NO_OPTIONAL_MEMBERS

#define JSCLASS_DEFAULT_WITH_CTOR(ctor) \
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, \
    nullptr, nullptr, nullptr, ctor, nullptr, nullptr, nullptr

#define JSPROP_DEFAULT	JSPROP_ENUMERATE|JSPROP_PERMANENT
#define JSPROP_STATIC	JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY

#define JSAPI_FUNC(name) JSBool name##(JSContext *cx, uintN argc, jsval *vp)
#define JSAPI_EMPTY_CTOR(name) JSBool name##(JSContext* cx, uintN argc, jsval* vp) { return JS_ThrowError(cx, #name " is not constructable."); }
#define JSAPI_PROP(name) JSBool name##(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
#define JSAPI_STRICT_PROP(name) JSBool name##(JSContext* cx, JSObject *obj, jsid id, JSBool strict, jsval* vp)

EXPORT JSBool JS_ThrowError(JSContext* cx, const char* message);
EXPORT void JS_DefineClasses(JSContext* cx, JSObject* obj, JSClassSpec* classes);

EXPORT JSBool JS_ArrayToVector(JSContext* cx, jsval arr, std::vector<jsval>& vec);
EXPORT JSBool JS_ArrayToVector(JSContext* cx, JSObject* arr, std::vector<jsval>& vec);

EXPORT JSObject* JS_VectorToArray(JSContext* cx, std::vector<jsval>& vec);
