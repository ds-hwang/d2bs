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
			proto = JS_GetProtoForClass(cx, obj, spec->proto);

		JS_InitClass(cx, obj, proto, spec->classp, spec->ctor, spec->argc,
			spec->properties, spec->methods, spec->static_properties, spec->static_methods);
	}
}

JSObject* JS_GetProtoForClass(JSContext* cx, JSObject* obj, JSClass* classp)
{
	// look up the proto on the given object
	jsval jsproto = JSVAL_NULL;
	if(JS_GetProperty(cx, obj, classp->name, &jsproto) && !JSVAL_IS_VOID(jsproto) && !JSVAL_IS_NULL(jsproto)) {
		JSObject* objproto = JSVAL_TO_OBJECT(jsproto);
		if(JS_ObjectIsFunction(cx, objproto))
			return objproto;
	}
	return nullptr;
}

JSObject* JS_NewObjectWithProto(JSContext* cx, JSObject* global, JSClassSpec* classp, JSClassSpec* proto, JSObject* parent)
{
	JSObject* obj = JS_NewObject(cx, classp->classp, JS_GetProtoForClass(cx, global, proto->classp), parent);

	if(proto->static_methods != nullptr) JS_DefineFunctions(cx, obj, proto->static_methods);
	if(proto->static_properties != nullptr) JS_DefineProperties(cx, obj, proto->static_properties);
	if(classp->static_methods != nullptr) JS_DefineFunctions(cx, obj, classp->static_methods);
	if(classp->static_properties != nullptr) JS_DefineProperties(cx, obj, classp->static_properties);

	if(proto->methods != nullptr) JS_DefineFunctions(cx, obj, proto->methods);
	if(proto->properties != nullptr) JS_DefineProperties(cx, obj, proto->properties);
	if(classp->methods != nullptr) JS_DefineFunctions(cx, obj, classp->methods);
	if(classp->properties != nullptr) JS_DefineProperties(cx, obj, classp->properties);

	return obj;
}
