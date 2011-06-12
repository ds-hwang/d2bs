#include "Stream.hpp"
#include "Socket.hpp"

#include <vector>

#include <boost/asio.hpp>

using namespace boost::asio::ip;

boost::asio::io_service service;

JSAPI_EMPTY_CTOR(Socket)
void socket_finalize(JSContext *cx, JSObject *obj);

JSClass socket_class = { "Socket", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_StrictPropertyStub, \
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, socket_finalize, \
    nullptr, nullptr, nullptr, Socket, nullptr, nullptr, nullptr };

JSClassSpec socket_spec = JS_CS(&socket_class, &stream_class, Socket, 0, socket_methods, socket_props, socket_static_methods, nullptr);

JSAPI_FUNC(socket_open);
JSAPI_FUNC(socket_connect);
JSAPI_FUNC(socket_read);
JSAPI_FUNC(socket_skip);
JSAPI_FUNC(socket_write);
JSAPI_FUNC(socket_close);

JSAPI_PROP(socket_get_length);
JSAPI_PROP(socket_get_readable);
JSAPI_PROP(socket_get_writable);
JSAPI_PROP(socket_get_skippable);
JSAPI_PROP(socket_get_closed);

JSFunctionSpec socket_methods[] = {
	JS_FS("connect", socket_connect, 2, JSPROP_STATIC),
	JS_FS("read", socket_read, 0, JSPROP_STATIC),
	JS_FS("write", socket_write, 0, JSPROP_STATIC),
	JS_FS("skip", socket_skip, 0, JSPROP_STATIC),
	JS_FS("close", socket_close, 0, JSPROP_STATIC),
	JS_FS_END
};

JSPropertySpec socket_props[] = {
	JS_PS("length", 0, JSPROP_STATIC, socket_get_length, nullptr),
	JS_PS("readable", 0, JSPROP_STATIC, socket_get_readable, nullptr),
	JS_PS("writable", 0, JSPROP_STATIC, socket_get_writable, nullptr),
	JS_PS("skippable", 0, JSPROP_STATIC, socket_get_skippable, nullptr),
	JS_PS("closed", 0, JSPROP_STATIC, socket_get_closed, nullptr),
	JS_PS_END
};

JSFunctionSpec socket_static_methods[] = {
	JS_FS("open", socket_open, 2, JSPROP_STATIC),
	JS_FS_END
};

JSAPI_FUNC(socket_open)
{
	jschar* jshost;
	uint16 port;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "Wc", &jshost, &port))
		return JS_FALSE;

	char p[6] = "";
	_itoa_s(port, p, 10);
	std::string host;
	host.assign(jshost, jshost+wcslen(jshost));

	auto resolver = tcp::resolver(service);
	auto query = tcp::resolver::query(host, p, tcp::resolver::query::numeric_service);
	auto it = resolver.resolve(query);
	auto end = tcp::resolver::iterator();

	auto socket = new tcp::socket(service);
	boost::system::error_code error = boost::asio::error::host_not_found;
	while(error && it != end) {
		((tcp::socket*)socket)->close();
		((tcp::socket*)socket)->connect(*it++, error);
	}
	if(error) return JS_ThrowError(cx, boost::system::system_error(error).what());

	JSObject* obj = JS_NewObjectWithProto(cx, JSVAL_TO_OBJECT(JS_THIS(cx, vp)), &socket_spec, &stream_spec, nullptr);
	if(!obj) {
		socket->close();
		delete socket;
		return JS_FALSE;
	}
	JS_DefineFunctions(cx, obj, socket_methods);
	JS_DefineProperties(cx, obj, socket_props);

	JS_SetPrivate(cx, obj, socket);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

	return JS_TRUE;
}

JSAPI_FUNC(socket_connect)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	jschar* jshost;
	uint16 port;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "Wc", &jshost, &port))
		return JS_FALSE;

	char p[6] = "";
	_itoa_s(port, p, 10);
	std::string host;
	host.assign(jshost, jshost+wcslen(jshost));

	auto resolver = tcp::resolver(service);
	auto query = tcp::resolver::query(host, p, tcp::resolver::query::numeric_service);
	auto it = resolver.resolve(query);
	auto end = tcp::resolver::iterator();

	boost::system::error_code error = boost::asio::error::host_not_found;
	while(error && it != end) {
		((tcp::socket*)socket)->close();
		((tcp::socket*)socket)->connect(*it++, error);
	}
	if(error) return JS_ThrowError(cx, boost::system::system_error(error).what());

	return JS_TRUE;
}

JSAPI_FUNC(socket_read)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	uintN size = 0;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "/u", &size))
		return JS_FALSE;

	if(size == 0) size = socket->available();
	try {
		std::vector<unsigned char> bytes(size);
		auto b = boost::asio::buffer(bytes);
		boost::asio::read(*socket, b);
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(JS_VectorToArray(cx, bytes)));
	} catch(boost::system::system_error e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_FUNC(socket_write)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	jsval* argv = JS_ARGV(cx, vp);
	JSObject* arr = nullptr;
	jschar* str = nullptr;
	if(!JS_ConvertArguments(cx, argc, argv, "o", &arr))
		return JS_FALSE;
	if(!JS_IsArrayObject(cx, arr)) {
		if(!JS_ConvertArguments(cx, argc, argv, "W", &str))
			return JS_FALSE;
		std::string cstr;
		cstr.assign(str, str+wcslen(str));

		std::vector<unsigned char> bytes(cstr.begin(), cstr.end());
		arr = JS_VectorToArray(cx, bytes);
	}

	try {
		std::vector<unsigned char> bytes;
		JS_ArrayToVector(cx, arr, bytes);
		auto b = boost::asio::buffer(bytes);
		boost::asio::write(*socket, b);
	} catch(boost::system::system_error e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_FUNC(socket_skip)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	uintN size = 1;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "/u", &size))
		return JS_FALSE;
	
	try {
		std::vector<unsigned char> bytes(size);
		auto b = boost::asio::buffer(bytes);
		boost::asio::read(*socket, b);
	} catch(boost::system::system_error e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_FUNC(socket_close)
{
	JSObject* obj = JSVAL_TO_OBJECT(JS_THIS(cx, vp));
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	try {
		socket->shutdown(tcp::socket::shutdown_both);
		socket->close();
	} catch(boost::system::system_error e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_PROP(socket_get_length)
{
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	try {
		JS_SET_RVAL(cx, vp, UINT_TO_JSVAL(socket->available()));
	} catch(boost::system::system_error e) { return JS_ThrowError(cx, e.what()); }

	return JS_TRUE;
}

JSAPI_PROP(socket_get_readable)
{
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(socket->is_open()));

	return JS_TRUE;
}

JSAPI_PROP(socket_get_writable)
{
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(socket->is_open()));

	return JS_TRUE;
}

JSAPI_PROP(socket_get_skippable)
{
	JS_SET_RVAL(cx, vp, JSVAL_FALSE);
	return JS_TRUE;
}

JSAPI_PROP(socket_get_closed)
{
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);

	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(socket->is_open()));

	return JS_TRUE;
}

void socket_finalize(JSContext *cx, JSObject *obj)
{
	tcp::socket* socket = (tcp::socket*)JS_GetPrivate(cx, obj);
	if(socket != nullptr) {
		socket->shutdown(tcp::socket::shutdown_both);
		socket->close();
		delete socket;
	}
}
