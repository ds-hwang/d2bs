#include "JSClasses.hpp"
#include "Modules.hpp"

JSAPI_EMPTY_CTOR(Global)

JSClass global = { "Global", JSCLASS_GLOBAL_FLAGS, JSCLASS_DEFAULT_WITH_CTOR(Global) };

JSClassSpec classes[] = {
	JS_CS(&global, nullptr, nullptr, nullptr, nullptr, nullptr),
	JS_CS_END
};
