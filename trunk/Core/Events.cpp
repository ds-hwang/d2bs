#include "Events.hpp"
#include "Script.hpp"

using namespace Core;

JSAPI_EMPTY_CTOR(Event);

JSFunctionSpec event_methods[] = {
	JS_FS("addListener", event_addListener, 2, JSPROP_DEFAULT),
	JS_FS("removeListener", event_removeListener, 2, JSPROP_DEFAULT),
	JS_FS("removeListeners", event_removeListeners, 1, JSPROP_DEFAULT),
	JS_FS_END
};

JSClass event_class = { "Event", JSCLASS_IS_GLOBAL, JSCLASS_DEFAULT_WITH_CTOR(::Event) };

JSClassSpec event_classes[] = {
	JS_CS(&event_class, nullptr, ::Event, 0, nullptr, nullptr, event_methods, nullptr),
	JS_CS_END
};

JSModuleSpec event_modules[] = {
	JS_MS("event", event_classes, nullptr, nullptr),
	JS_MS_END
};

JSAPI_FUNC(event_addListener)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	JSString* earg = nullptr;
	JSFunction* f = nullptr;
	jsval func = JS_ARGV(cx, vp)[1];
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "Sf", &earg, &f))
		return JS_FALSE;

	script->AddListener(JS_GetStringCharsZ(cx, earg), func);

	return JS_TRUE;
}

JSAPI_FUNC(event_removeListener)
{
	return JS_TRUE;
}

JSAPI_FUNC(event_removeListeners)
{
	return JS_TRUE;
}

JSAPI_FUNC(event_fire)
{
	return JS_TRUE;
}
