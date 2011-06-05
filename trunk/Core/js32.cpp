#include "js32.hpp"
#include <cstdarg>

JSBool JS_ThrowError(JSContext* cx, const char* message, ...)
{
	va_list args;
	va_start(args, message);

	int len = _vscprintf(message, args);
	char* msg = new char[len+1];
	memset(msg, 0, len+1);
	vsprintf_s(msg, len+1, message, args);

	va_end(args);

	JS_ReportError(cx, msg);
	delete[] msg;
	return JS_FALSE;
}

void JS_DefineClasses(JSContext* cx, JSObject* obj, JSClassSpec* classes)
{
	for(JSClassSpec* spec = classes; spec->classp != nullptr; spec++)
	{
		JSObject* proto = nullptr;
		if(spec->proto != nullptr)
		{
			// look up the proto on the given object
			jsval jsproto = JSVAL_NULL;
			if(JS_GetProperty(cx, obj, spec->proto->name, &jsproto)) {
				JSObject* objproto = JSVAL_TO_OBJECT(jsproto);
				if(JS_ObjectIsFunction(cx, objproto) && JS_ObjectIsFunction(cx, objproto))
					proto = objproto;
			}
		}

		JS_InitClass(cx, obj, proto, spec->classp,
			spec->classp->construct, 0, spec->properties, spec->methods,
			spec->static_properties, spec->static_methods);
	}
}

JSBool JS_ArrayToVector(JSContext* cx, jsval arr, std::vector<jsval>& vec)
{
	return JS_ArrayToVector(cx, JSVAL_TO_OBJECT(arr), vec);
}
JSBool JS_ArrayToVector(JSContext* cx, JSObject* arr, std::vector<jsval>& vec)
{
	if(!JS_IsArrayObject(cx, arr))
		return JS_ThrowError(cx, "Object must be an Array");

	jsuint len = 0;
	JS_GetArrayLength(cx, arr, &len);
	for(jsuint i = 0; i < len; i++)
	{
		jsval el = JSVAL_VOID;
		JS_GetElement(cx, arr, i, &el);
		vec.push_back(el);
	}
	return JS_TRUE;
}

JSObject* JS_VectorToArray(JSContext* cx, std::vector<jsval>& vec)
{
	size_t len = vec.size();
	jsval* jsarr = new jsval[len];

	auto end = vec.end();
	int i = 0;
	for(auto it = vec.begin(); it != end; it++, i++) jsarr[i] = vec[i];

	JSObject* arr = JS_NewArrayObject(cx, len, jsarr);
	return arr;
}
