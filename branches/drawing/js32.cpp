#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "js32.h"
#include "windows.h"
#include "Script.h"
#include "ScriptEngine.h"

using namespace std;

JSBool JS_ConvertArgumentsEx(JSContext* cx, uintN argc, jsval* argv, const char* format, ...)
{
	int formatLen = strlen(format);
	bool optional = false;
	va_list args;
	va_start(args, format);
	JSBool result = JS_TRUE;

	JS_EnterLocalRootScope(cx);
	jsval* arg = argv;
	uintN argPos = 0;

#define CHECK(type, code) if(JS_TypeOfValue(cx, *arg) != type || !code) result = JS_FALSE;

	for(int i = 0; i < formatLen; i++)
	{
		switch(format[i])
		{
			case '/':
				optional = true;
				continue;
			case 'b':
				CHECK(JSTYPE_BOOLEAN, JS_ValueToBoolean(cx, *arg, va_arg(args, JSBool*)))
				break;
			case 'c':
				CHECK(JSTYPE_NUMBER, JS_ValueToUint16(cx, *arg, va_arg(args, uint16*)))
				break;
			case 'i':
				CHECK(JSTYPE_NUMBER, JS_ValueToECMAInt32(cx, *arg, va_arg(args, int32*)))
				break;
			case 'u':
				CHECK(JSTYPE_NUMBER, JS_ValueToECMAUint32(cx, *arg, va_arg(args, uint32*)))
				break;
			case 'd':
				CHECK(JSTYPE_NUMBER, JS_ValueToNumber(cx, *arg, va_arg(args, jsdouble*)))
				break;
			case 's':
			{
				if(JS_TypeOfValue(cx, *arg) != JSTYPE_STRING)
					result = JS_FALSE;
				else
				{
					JSString* str = JS_ValueToString(cx, *arg);
					if(str == NULL)
						result = JS_FALSE;
					else
						va_arg(args, char*) = JS_GetStringBytes(str);
				}
			}
				break;
			case 'w':
			{
				if(JS_TypeOfValue(cx, *arg) != JSTYPE_STRING)
					result = JS_FALSE;
				else
				{
					JSString* str = JS_ValueToString(cx, *arg);
					if(str == NULL)
						result = JS_FALSE;
					else
						va_arg(args, jschar*) = JS_GetStringChars(str);
				}
			}
				break;
			case 'o':
				CHECK(JSTYPE_OBJECT, JS_ValueToObject(cx, *arg, &(va_arg(args, JSObject*))))
				break;
			case 'O':
			{
				// only typecheck the object
				if(!JS_TypeOfValue(cx, *arg) != JSTYPE_OBJECT)
					result = JS_FALSE;
				else
					va_arg(args, jsval*) = arg;
			}
				break;
			case 'n':
				if(*arg != JSVAL_NULL)
					result = JS_FALSE;
				else
					va_arg(args, jsval*) = arg;
			case 'f':
				CHECK(JSTYPE_FUNCTION, JS_ConvertValue(cx, *arg, JSTYPE_FUNCTION, va_arg(args, jsval*)))
				break;
		}
		arg++;
		argPos++;

		if(argPos == argc)
			if((i+1) != formatLen && !optional) result = JS_FALSE;
			else break;

		if(result == JS_FALSE)
			break;
	}

#undef CHECK

	JS_LeaveLocalRootScope(cx);
	va_end(args);
	return result;
}

JSBool ThrowJSError(JSContext* cx, JSObject* obj, const char* format, ...)
{
	char msg[2048];
	va_list args;
	va_start(args, format);
	vsprintf_s(msg, 2048, format, args);
	va_end(args);

	if(JS_GetContextThread(cx) != (jsword)GetCurrentThreadId()) {
		JS_ReportError(cx, msg);
		return JS_FALSE;
	}

	JS_BeginRequest(cx);
	JSString* jsstr = JS_NewStringCopyZ(cx, msg);
	if(!jsstr)
	{
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	if(JS_AddRoot(&jsstr) == JS_FALSE)
	{
		JS_EndRequest(cx);
		return JS_FALSE;
	}

	const char* ccargs[] = {"msg"};
	const char* body = "throw new Error(msg);";
	JSFunction* func = JS_CompileFunction(cx, obj, NULL, 1, ccargs, body, strlen(body), NULL, 0);
	if(!func)
	{
		JS_RemoveRoot(&jsstr);
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	JSObject* funcObj = JS_GetFunctionObject(func);
	if(!funcObj)
	{
		JS_RemoveRoot(&jsstr);
		JS_EndRequest(cx);
		return JS_FALSE;
	}
	if(JS_AddRoot(&funcObj) == JS_FALSE)
	{
		JS_RemoveRoot(&jsstr);
		JS_EndRequest(cx);
		return JS_FALSE;
	}

	jsval dummy;
	jsval jsargs[]={STRING_TO_JSVAL(jsstr)};
	JS_CallFunction(cx, obj, func, 1, jsargs, &dummy);

	JS_RemoveRoot(&jsstr);
	JS_RemoveRoot(&funcObj);

	JS_EndRequest(cx);

	return JS_FALSE;
}

JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv, JSObject* proto, JSObject* parent)
{
	if(JS_GetContextThread(cx) == 0)
		JS_SetContextThread(cx);
	JSObject* obj = JS_NewObject(cx, classp, proto, parent);

	if(obj)
	{
		// add root to avoid newborn root problem
		if(JS_AddRoot(&obj) == JS_FALSE)
			return NULL;
		if(obj && funcs && !JS_DefineFunctions(cx, obj, funcs))
			obj = NULL;
		if(obj && props && !JS_DefineProperties(cx, obj, props))
			obj = NULL;
		if(obj)
			JS_SetPrivate(cx, obj, priv);
		JS_RemoveRoot(&obj);
	}
	return obj;
}

