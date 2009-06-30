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

//Oh here there
	if(JS_GetContextThread(cx) != GetCurrentThreadId()) {
//Oh here there
		JS_ReportError(cx, msg);
//Oh here there
		return JS_FALSE;
	}

//Oh here there
	JS_BeginRequest(cx);
//Oh here there
	JSString* jsstr = JS_NewStringCopyZ(cx, msg);
	if(!jsstr)
//Oh here there
		return JS_FALSE;
//Oh here there
	JS_AddNamedRoot(cx, &jsstr, "errstr");

	const char* ccargs[] = {"msg"};
	const char* body = "throw new Error(msg);";
//Oh here there
	JSFunction* func = JS_CompileFunction(cx, obj, NULL, 1, ccargs, body, strlen(body), NULL, 0);
//Oh here there
	JSObject* funcObj = JS_GetFunctionObject(func);
	if(!funcObj)
//Oh here there
		return JS_FALSE;
//Oh here there
	JS_AddNamedRoot(cx, &funcObj, "error function");

	jsval dummy;
	jsval jsargs[]={STRING_TO_JSVAL(jsstr)};
//Oh here there
	JS_CallFunction(cx, obj, func, 1, jsargs, &dummy);

//Oh here there
	JS_RemoveRoot(cx, &jsstr);
//Oh here there
	JS_RemoveRoot(cx, &funcObj);

//Oh here there
	JS_EndRequest(cx);

//Oh here there
	return JS_FALSE;
}

JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv, JSObject* proto, JSObject* parent)
{
	// TODO: determine how to block this when the GC is running...
//Oh here there
	JS_SetContextThread(cx);
//Oh here there
	JS_BeginRequest(cx);
//Oh here there
	JSObject* obj = JS_NewObject(cx, classp, proto, parent);

	if(obj)
	{
		// add root to avoid newborn root problem
//Oh here there
		JS_AddRoot(cx, &obj);

//Oh here there
		if(obj && funcs && !JS_DefineFunctions(cx, obj, funcs))
		{
			obj = NULL;
		}
//Oh here there
		if(obj && props && !JS_DefineProperties(cx, obj, props))
		{
			obj = NULL;
		}
		if(obj)
		{
//Oh here there
			JS_SetPrivate(cx, obj, priv);
		}
//Oh here there
		JS_RemoveRoot(cx, &obj);
	}
//Oh here there
	JS_EndRequest(cx);
	// we dont want to clear the context.
//Oh here there
	// JS_ClearContextThread(cx);
	return obj;
}

JSContext* BuildContext(JSRuntime* runtime)
{
	// 8192 (0x2000) is the recommended default and shouldn't be tinkered with
//Oh here there
	JSContext* context = JS_NewContext(runtime, 0x2000);
	if(!context)
		throw std::exception("Couldn't create the context");
//Oh here there
	JS_SetErrorReporter(context, reportError);
//Oh here there
	JS_SetBranchCallback(context, branchCallback);
//Oh here there
	JS_SetOptions(context, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML|JSOPTION_NATIVE_BRANCH_CALLBACK);
//Oh here there
	JS_SetVersion(context, JSVERSION_1_7);
	return context;
}