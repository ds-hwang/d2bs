#pragma once

#define XP_WIN

#include "jsapi.h"

#define JSCLASS_DEFAULT_STANDARD_MEMBERS \
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub, \
    JSCLASS_NO_OPTIONAL_MEMBERS

#define JSAPI_FUNC(name) JSBool name##(JSContext *cx, uintN argc, jsval *vp)

