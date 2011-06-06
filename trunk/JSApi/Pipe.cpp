#include "Pipe.hpp"

#include "Stream.hpp"

JSAPI_EMPTY_CTOR(Pipe)

JSClass pipe_class = { "Pipe", JSCLASS_HAS_PRIVATE, JSCLASS_DEFAULT_WITH_CTOR(Pipe) };

JSClassSpec pipe_spec = JS_CS(&pipe_class, &stream_class, Pipe, 0, pipe_methods, pipe_props, pipe_static_methods, nullptr);

JSFunctionSpec pipe_methods[] = {
	JS_FS_END
};

JSPropertySpec pipe_props[] = {
	JS_PS_END
};

JSFunctionSpec pipe_static_methods[] = {
	JS_FS_END
};
