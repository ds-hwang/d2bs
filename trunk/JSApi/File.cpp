#include "File.hpp"

#include "Stream.hpp"

JSAPI_EMPTY_CTOR(File)

JSClass file_class = { "File", JSCLASS_HAS_PRIVATE, JSCLASS_DEFAULT_WITH_CTOR(File) };

JSClassSpec file_spec = JS_CS(&file_class, &stream_class, File, 0, file_methods, file_props, file_static_methods, nullptr);

JSFunctionSpec file_methods[] = {
	JS_FS_END
};

JSPropertySpec file_props[] = {
	JS_PS_END
};

JSFunctionSpec file_static_methods[] = {
	JS_FS_END
};
