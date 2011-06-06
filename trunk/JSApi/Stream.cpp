#include "js32.hpp"

#include "Stream.hpp"
#include "File.hpp"
#include "Socket.hpp"
#include "Pipe.hpp"

JSAPI_EMPTY_CTOR(Stream);

JSAPI_FUNC(stream_read) { return JS_ThrowError(cx, "This stream does not support reading"); }
JSAPI_FUNC(stream_write){ return JS_ThrowError(cx, "This stream does not support writing"); }
JSAPI_FUNC(stream_skip) { return JS_ThrowError(cx, "This stream does not support skipping"); }
JSAPI_FUNC(stream_flush){ return JS_ThrowError(cx, "This stream does not support flushing"); }
JSAPI_FUNC(stream_close){ return JS_ThrowError(cx, "This stream does not support closing"); }

JSAPI_PROP(stream_get_length)    { return JS_ThrowError(cx, "This stream does not support the length property"); }
JSAPI_PROP(stream_get_position)  { return JS_ThrowError(cx, "This stream does not support the position property"); }
JSAPI_PROP(stream_get_readable)  { return JS_ThrowError(cx, "This stream does not support the readable property"); }
JSAPI_PROP(stream_get_writable)  { return JS_ThrowError(cx, "This stream does not support the writable property"); }
JSAPI_PROP(stream_get_skippable) { return JS_ThrowError(cx, "This stream does not support the skippable property"); }
JSAPI_PROP(stream_get_closed)    { return JS_ThrowError(cx, "This stream does not support the closed property"); }
JSAPI_STRICT_PROP(stream_set_position)  { return JS_ThrowError(cx, "This stream does not support the position property"); }

JSClass stream_class = { "Stream", 0, JSCLASS_DEFAULT_WITH_CTOR(Stream) };

JSClassSpec stream_spec = JS_CS(&stream_class, nullptr, Stream, 0, stream_methods, stream_props, nullptr, nullptr);

JSFunctionSpec stream_methods[] = {
	JS_FS("read", stream_read, 0, JSPROP_DEFAULT),
	JS_FS("write", stream_write, 0, JSPROP_DEFAULT),
	JS_FS("skip", stream_skip, 0, JSPROP_DEFAULT),
	JS_FS("flush", stream_flush, 0, JSPROP_DEFAULT),
	JS_FS("close", stream_close, 0, JSPROP_DEFAULT),
	JS_FS_END
};

JSPropertySpec stream_props[] = {
	JS_PS("length", 0, JSPROP_STATIC, stream_get_length, nullptr),
	JS_PS("position", 0, JSPROP_DEFAULT, stream_get_position, stream_set_position),
	JS_PS("readable", 0, JSPROP_STATIC, stream_get_readable, nullptr),
	JS_PS("writable", 0, JSPROP_STATIC, stream_get_writable, nullptr),
	JS_PS("skippable", 0, JSPROP_STATIC, stream_get_skippable, nullptr),
	JS_PS("closed", 0, JSPROP_STATIC, stream_get_closed, nullptr),
	JS_PS_END
};

JSClassSpec stream_classes[] = {
	stream_spec,
	file_spec,
	socket_spec,
	pipe_spec,
	JS_CS_END
};

JSModuleSpec stream_modules[] = {
	JS_MS("stream", stream_classes, nullptr, nullptr),
	JS_MS_END
};
