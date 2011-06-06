#include "Events.hpp"
#include "Script.hpp"

using namespace Core;

JSAPI_EMPTY_CTOR(Event);

JSFunctionSpec event_methods[] = {
	JS_FS("addListener", event_addListener, 2, JSPROP_STATIC),
	JS_FS("removeListener", event_removeListener, 2, JSPROP_STATIC),
	JS_FS("removeListeners", event_removeListeners, 1, JSPROP_STATIC),
	JS_FS("fire", event_fire, 1, JSPROP_STATIC),
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
	Script* script = (Script*)JS_GetContextPrivate(cx);

	JSString* earg = nullptr;
	JSFunction* f = nullptr;
	jsval func = JS_ARGV(cx, vp)[1];
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "Sf", &earg, &f))
		return JS_FALSE;

	script->RemoveListener(JS_GetStringCharsZ(cx, earg), func);

	return JS_TRUE;
}

JSAPI_FUNC(event_removeListeners)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	JSString* earg = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "/S", &earg))
		return JS_FALSE;

	JSBool empty = JS_FALSE;
	if(!JS_StringEqualsAscii(cx, earg, "", &empty))
		return JS_FALSE;
	if(empty) script->RemoveListeners();
	else script->RemoveListeners(JS_GetStringCharsZ(cx, earg));

	return JS_TRUE;
}

JSAPI_FUNC(event_fire)
{
	// TODO: make this able to handle blocking events
	Script* script = (Script*)JS_GetContextPrivate(cx);

	JSString* earg = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &earg))
		return JS_FALSE;

	Core::Event* evt = new Core::Event;
	evt->blocker = INVALID_HANDLE_VALUE;
	evt->evtName = JS_GetStringCharsZ(cx, earg);
	evt->result = false;
	evt->args = ArgumentList();

	if(argc > 1) {
		jsval* argv = &JS_ARGV(cx, vp)[1];
		for(uintN i = 0; i < argc-1; i++)
			evt->args.push_back(boost::any(boost::shared_ptr<JSAutoRoot>(new JSAutoRoot(cx, argv[i]))));
	}

	script->FireEvent(evt);

	return JS_TRUE;
}
