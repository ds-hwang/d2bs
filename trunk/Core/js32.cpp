#include "js32.hpp"

JSBool JS_ThrowError(JSContext* cx, const char* message)
{
	JS_ReportError(cx, message);
	return JS_FALSE;
}

void JS_DefineClasses(JSContext* cx, JSObject* obj, JSClassSpec* classes)
{
	for(JSClassSpec* clasp = classes; clasp->classp != nullptr; clasp++)
	{
		JSObject* proto = nullptr;
		if(clasp->proto != nullptr)
		{
			// look up the proto on the global object
			jsval jsproto = JSVAL_NULL;
			if(JS_GetProperty(cx, obj, clasp->proto->name, &jsproto))
				if(!JSVAL_IS_NULL(jsproto) && !JSVAL_IS_VOID(jsproto))
					proto = JSVAL_TO_OBJECT(jsproto);
		}

		JS_InitClass(cx, obj, proto, clasp->classp,
			clasp->classp->construct, 0, clasp->properties, clasp->methods,
			clasp->static_properties, clasp->static_methods);
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

EXPORT JSObject* JS_VectorToArray(JSContext* cx, std::vector<jsval>& vec)
{
	size_t len = vec.size();
	jsval* jsarr = new jsval[len];

	auto end = vec.end();
	int i = 0;
	for(auto it = vec.begin(); it != end; it++, i++) jsarr[i] = vec[i];

	JSObject* arr = JS_NewArrayObject(cx, len, jsarr);
	return arr;
}
