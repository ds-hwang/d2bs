#include "js32.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include "windows.h"
#include "Script.h"
#include "ScriptEngine.h"

#include "debugnew/debug_new.h"

using namespace std;

JSBool ThrowJSError(JSContext* cx, JSObject* obj, const char* format, ...)
{
	char msg[2048];
	va_list args;
	va_start(args, format);
	vsprintf_s(msg, 2048, format, args);
	va_end(args);

	if(JS_GetContextThread(cx) != GetCurrentThreadId()) {
		JS_ReportError(cx, msg);
		return JS_FALSE;
	}

	JS_BeginRequest(cx);
	JSString* jsstr = JS_NewStringCopyZ(cx, msg);
	if(!jsstr)
		return JS_FALSE;
	JS_AddNamedRoot(cx, &jsstr, "errstr");

	const char* ccargs[] = {"msg"};
	const char* body = "throw new Error(msg);";
	JSFunction* func = JS_CompileFunction(cx, obj, NULL, 1, ccargs, body, strlen(body), NULL, 0);
	JSObject* funcObj = JS_GetFunctionObject(func);
	if(!funcObj)
		return JS_FALSE;
	JS_AddNamedRoot(cx, &funcObj, "error function");

	jsval dummy;
	jsval jsargs[]={STRING_TO_JSVAL(jsstr)};
	JS_CallFunction(cx, obj, func, 1, jsargs, &dummy);

	JS_RemoveRoot(cx, &jsstr);
	JS_RemoveRoot(cx, &funcObj);

	JS_EndRequest(cx);

	return JS_FALSE;
}

// CALLERS: This must be called from within JSAPI_DO_REQUEST
JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv, JSObject* proto, JSObject* parent)
{
	JSObject* obj = JS_NewObject(cx, classp, proto, parent);

	if(obj)
	{
		// add root to avoid newborn root problem
		JS_AddRoot(cx, &obj);

		if(obj && funcs && !JS_DefineFunctions(cx, obj, funcs))
		{
			obj = NULL;
		}
		if(obj && props && !JS_DefineProperties(cx, obj, props))
		{
			obj = NULL;
		}
		if(obj)
		{
			JS_SetPrivate(cx, obj, priv);
		}
		JS_RemoveRoot(cx, &obj);
	}

	return obj;
}
