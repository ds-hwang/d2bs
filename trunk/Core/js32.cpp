#include "js32.hpp"

JSBool JS_ThrowError(JSContext* cx, JSObject* obj, const char* message)
{
	return JS_FALSE;
}

void JS_DefineClasses(JSContext* cx, JSObject* obj, JSClassSpec* classes)
{
	for(JSClassSpec* clasp = classes; clasp->classp != NULL; clasp++)
	{
		JSObject* proto = NULL;
		if(clasp->proto != NULL)
		{
			// look up the proto on the global object
			jsval jsproto = JSVAL_NULL;
			if(JS_GetProperty(cx, obj, clasp->proto->name, &jsproto))
				if(jsproto != JSVAL_NULL && jsproto != JSVAL_VOID)
					proto = JSVAL_TO_OBJECT(jsproto);
		}

		JS_InitClass(cx, obj, proto, clasp->classp,
			clasp->classp->construct, 0, clasp->properties, clasp->methods,
			clasp->static_properties, clasp->static_methods);
	}
}
