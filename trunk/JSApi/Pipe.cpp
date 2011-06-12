#include "Pipe.hpp"

#include "Stream.hpp"

#include <Windows.h>

JSAPI_EMPTY_CTOR(Pipe)
void pipe_finalize(JSContext* cx, JSObject* obj);

JSClass pipe_class = { "Pipe", JSCLASS_HAS_PRIVATE, 
	JSCLASS_SPEC(JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub,
				 JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, pipe_finalize, Pipe)};

JSClassSpec pipe_spec = JS_CS(&pipe_class, &stream_class, Pipe, 0, pipe_methods, pipe_props, pipe_static_methods, nullptr);

JSAPI_FUNC(pipe_open);
JSAPI_FUNC(pipe_connect);
JSAPI_FUNC(pipe_read);
JSAPI_FUNC(pipe_write);
JSAPI_FUNC(pipe_skip);
JSAPI_FUNC(pipe_close);

JSAPI_PROP(pipe_get_readable);
JSAPI_PROP(pipe_get_writable);
JSAPI_PROP(pipe_get_skippable);
JSAPI_PROP(pipe_get_closed);

JSFunctionSpec pipe_methods[] = {
	JS_FS("connect", pipe_connect, 0, JSPROP_STATIC),
	JS_FS("read", pipe_read, 1, JSPROP_STATIC),
	JS_FS("write", pipe_write, 1, JSPROP_STATIC),
	JS_FS("skip", pipe_skip, 1, JSPROP_STATIC),
	JS_FS("close", pipe_close, 0, JSPROP_STATIC),
	JS_FS_END
};

JSPropertySpec pipe_props[] = {
	JS_PS("readable", 0, JSPROP_STATIC, pipe_get_readable, nullptr),
	JS_PS("writable", 0, JSPROP_STATIC, pipe_get_writable, nullptr),
	JS_PS("skippable", 0, JSPROP_STATIC, pipe_get_skippable, nullptr),
	JS_PS("closed", 0, JSPROP_STATIC, pipe_get_closed, nullptr),
	JS_PS_END
};

JSFunctionSpec pipe_static_methods[] = {
	JS_FS("open", pipe_open, 2, JSPROP_STATIC),
	JS_FS_END
};

JSAPI_FUNC(pipe_open)
{
	JSString* fname = nullptr;
	JSBool append = JS_FALSE;

	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S/b", &fname, &append))
		return JS_FALSE;

	HANDLE pipe = INVALID_HANDLE_VALUE;
	jschar name[255];
	swprintf_s(name, L"\\\\.\\pipe\\%s", JS_GetStringCharsZ(cx, fname));
	pipe = CreateNamedPipeW(name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_NOWAIT,
				PIPE_UNLIMITED_INSTANCES, 1024 , 1024, 0, nullptr);
	if(pipe == INVALID_HANDLE_VALUE)
		return JS_ThrowError(cx, "Failed to open pipe: %x", GetLastError());

	JSObject* obj = JS_NewObjectWithProto(cx, JSVAL_TO_OBJECT(JS_THIS(cx, vp)), &pipe_spec, &stream_spec, nullptr);
	if(!obj) {
		CloseHandle(pipe);
		return JS_FALSE;
	}
	JS_DefineFunctions(cx, obj, pipe_methods);
	JS_DefineProperties(cx, obj, pipe_props);

	JS_SetPrivate(cx, obj, pipe);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

	return JS_TRUE;
}

JSAPI_FUNC(pipe_connect)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	if(pipe == INVALID_HANDLE_VALUE) return JS_ThrowError(cx, "Pipe is closed");

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(ConnectNamedPipe(pipe, nullptr) ?
				TRUE : GetLastError() == ERROR_PIPE_CONNECTED));

	return JS_TRUE;
}

JSAPI_FUNC(pipe_read)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	if(pipe == INVALID_HANDLE_VALUE) return JS_ThrowError(cx, "Pipe is closed");

	std::wstring line;
	jschar buf[1024];
	DWORD size = 0;

	ReadFile(pipe, buf, 1024, &size, nullptr);
	line.assign(buf, size);

	JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(JS_NewUCStringCopyZ(cx, line.c_str())));

	return JS_TRUE;
}

JSAPI_FUNC(pipe_write)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	if(pipe == INVALID_HANDLE_VALUE) return JS_ThrowError(cx, "Pipe is closed");

	JSString* line = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &line))
		return JS_FALSE;

	size_t size;
	DWORD count;
	const jschar* str = JS_GetStringCharsAndLength(cx, line, &size);
	WriteFile(pipe, str, size, &count, nullptr);

	return JS_TRUE;
}

JSAPI_FUNC(pipe_skip)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	if(pipe == INVALID_HANDLE_VALUE) return JS_ThrowError(cx, "Pipe is closed");

	jschar buf[1024];
	DWORD size = 0;

	ReadFile(pipe, buf, 1024, &size, nullptr);

	return JS_TRUE;
}

JSAPI_FUNC(pipe_close)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	if(pipe == INVALID_HANDLE_VALUE) return JS_ThrowError(cx, "Pipe is closed");

	CloseHandle(pipe);
	JS_SetPrivate(cx, obj, INVALID_HANDLE_VALUE);

	return JS_ThrowError(cx, "This stream does not support closing");
}

JSAPI_PROP(pipe_get_readable)
{
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(pipe != INVALID_HANDLE_VALUE));
	return JS_TRUE;
}

JSAPI_PROP(pipe_get_writable)
{
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(pipe != INVALID_HANDLE_VALUE));
	return JS_TRUE;
}

JSAPI_PROP(pipe_get_skippable)
{
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(pipe != INVALID_HANDLE_VALUE));
	return JS_TRUE;
}

JSAPI_PROP(pipe_get_closed)
{
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(pipe == INVALID_HANDLE_VALUE));
	return JS_TRUE;
}

void pipe_finalize(JSContext *cx, JSObject *obj)
{
	HANDLE pipe = (HANDLE)JS_GetPrivate(cx, obj);
	if(pipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pipe);
	}
}
