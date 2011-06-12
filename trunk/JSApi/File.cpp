#include "File.hpp"

#include "Stream.hpp"

#include <fstream>
#include <string>

JSAPI_EMPTY_CTOR(File)
void file_finalize(JSContext *cx, JSObject *obj);

JSClass file_class = { "File", JSCLASS_HAS_PRIVATE,
	JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
				 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, file_finalize, File) };

JSClassSpec file_spec = JS_CS(&file_class, &stream_class, File, 0, file_methods, file_props, file_static_methods, nullptr);

JSAPI_FUNC(file_open);
JSAPI_FUNC(file_read);
JSAPI_FUNC(file_write);
JSAPI_FUNC(file_skip);
JSAPI_FUNC(file_flush);
JSAPI_FUNC(file_close);

JSAPI_PROP(file_get_length);
JSAPI_PROP(file_get_position);
JSAPI_PROP(file_get_readable);
JSAPI_PROP(file_get_writable);
JSAPI_PROP(file_get_skippable);
JSAPI_PROP(file_get_closed);
JSAPI_STRICT_PROP(file_set_position);

JSFunctionSpec file_methods[] = {
	JS_FS("read", file_read, 1, JSPROP_STATIC),
	JS_FS("write", file_write, 1, JSPROP_STATIC),
	JS_FS("skip", file_skip, 1, JSPROP_STATIC),
	JS_FS("flush", file_flush, 0, JSPROP_STATIC),
	JS_FS("close", file_close, 0, JSPROP_STATIC),
	JS_FS_END
};

JSPropertySpec file_props[] = {
	JS_PS("length", 0, JSPROP_STATIC, file_get_length, nullptr),
	JS_PS("position", 0, JSPROP_DEFAULT, file_get_position, file_set_position),
	JS_PS("readable", 0, JSPROP_STATIC, file_get_readable, nullptr),
	JS_PS("writable", 0, JSPROP_STATIC, file_get_writable, nullptr),
	JS_PS("skippable", 0, JSPROP_STATIC, file_get_skippable, nullptr),
	JS_PS("closed", 0, JSPROP_STATIC, file_get_closed, nullptr),
	JS_PS_END
};

JSFunctionSpec file_static_methods[] = {
	JS_FS("open", file_open, 2, JSPROP_STATIC),
	JS_FS_END
};

JSAPI_FUNC(file_open)
{
	JSString* fname = nullptr;
	JSBool append = JS_FALSE;

	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S/b", &fname, &append))
		return JS_FALSE;

	auto mode = std::ios::in|std::ios::out;
	if(append) mode |= std::ios::app;

	std::wfstream* file = nullptr;
	try { file = new std::wfstream(JS_GetStringCharsZ(cx, fname), mode); }
	catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	JSObject* obj = JS_NewObjectWithProto(cx, JSVAL_TO_OBJECT(JS_THIS(cx, vp)), &file_spec, &stream_spec, nullptr);
	if(!obj) {
		file->close();
		delete file;
		return JS_FALSE;
	}
	JS_DefineFunctions(cx, obj, file_methods);
	JS_DefineProperties(cx, obj, file_props);

	JS_SetPrivate(cx, obj, file);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

	return JS_TRUE;
}

JSAPI_FUNC(file_read)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);
	std::wstring line;

	try { std::getline(*file, line); }
	catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, line.c_str())));

	return JS_TRUE;
}

JSAPI_FUNC(file_write)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	JSString* line = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &line))
		return JS_FALSE;

	try {
		(*file) << JS_GetStringCharsZ(cx, line);
		file->flush();
	} catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_FUNC(file_skip)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	uint32 count = 0;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "u", &count))
		return JS_FALSE;

	try {
		wchar_t* buf = new wchar_t[count];
		file->read(buf, count);
	} catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_FUNC(file_flush)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	try { file->flush(); }
	catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_FUNC(file_close)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	try { file->close(); }
	catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_PROP(file_get_length)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	try {
		auto pos = file->tellg();
		file->seekg(std::ios::beg);
		auto begin = file->tellg();
		file->seekg(std::ios::end);
		auto end = file->tellg();
		file->seekg(pos);

		uint32 size = end - begin;
		JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(size));
	} catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_PROP(file_get_position)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	try { JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(file->tellg())); }
	catch(std::exception& e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_PROP(file_get_readable)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(file->good() && !file->eof()));
	return JS_TRUE;
}

JSAPI_PROP(file_get_writable)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(file->good()));
	return JS_TRUE;
}

JSAPI_PROP(file_get_skippable)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(file->good() && !file->eof()));
	return JS_TRUE;
}

JSAPI_PROP(file_get_closed)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(!file->is_open()));
	return JS_TRUE;
}

JSAPI_STRICT_PROP(file_set_position)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);

	int count = JSVAL_TO_INT(*vp);

	file->seekg(count, std::ios::beg);

	return JS_TRUE;
}

void file_finalize(JSContext *cx, JSObject *obj)
{
	std::wfstream* file = (std::wfstream*)JS_GetPrivate(cx, obj);
	if(file != nullptr)
	{
		file->close();
		delete file;
	}
}
