#include "js32.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include "windows.h"

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

JSObject* BuildObject(JSContext* cx, JSClass* classp, JSFunctionSpec* funcs, JSPropertySpec* props, void* priv, JSObject* proto, JSObject* parent)
{

	JSObject* obj = JS_NewObject(cx, classp, proto, parent);
	if(obj)
	{
		// add root to avoid newborn root problem
		JS_AddRoot(cx, &obj);
		if(funcs && !JS_DefineFunctions(cx, obj, funcs))
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
			JS_RemoveRoot(cx, &obj);
		}
	}
	return obj;
}

JSBool JS_ConvertArgumentsEx(JSContext* cx, uintN argc, jsval* argv, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	JSBool result = JS_ConvertArgumentsExVA(cx, argc, argv, format, args);
	va_end(args);
	return result;
}

/*
 * Format is a string of the following characters (spaces are insignificant),
 * specifying the tabulated type conversions:
 *
 *   b      JSBool          Boolean
 *   c      uint16/jschar   ECMA uint16, Unicode char
 *   i      int32           ECMA int32
 *   u      uint32          ECMA uint32
 *   j      int32           Rounded int32 (coordinate)
 *   d      jsdouble        IEEE double
 *   I      jsdouble        Integral IEEE double
 *   s      char *          C string
 *   S      JSString *      Unicode string, accessed by a JSString pointer
 *   W      jschar *        Unicode character vector, 0-terminated (W for wide)
 *   o      JSObject *      Object reference
 *   f      JSFunction *    Function private
 *   F      jsval           Function as jsval (type checked, not converted)
 *   v      jsval           Argument value (no conversion)
 *   *      N/A             Skip this argument (no vararg)
 *   /      N/A             End of required arguments
 */

JSBool JS_ConvertArgumentsExVA(JSContext* cx, uintN argc, jsval* argv, const char* format, va_list args)
{
/*	// TODO: Make this throw real exceptions if possible... might not be because of the js engine
	int i = 0;
	int len = strlen(format);
	char* c = format, msg[1024], error_msg[] = "Argument %d doesn't match expected type %s";
	while(*c != '\0' && i <= argc)
	{
		if(i == argc)
		{
			for(int j = 0; j < len; j++)
				if(
			JS_ReportError(cx, "Expected %d arguments, %d found.", , argc);
		}
		char cx = *c;
		c++;
		if(isspace(cx))
			continue;
/*
 * Format is a string of the following characters (spaces are insignificant),
 * specifying the tabulated type conversions:
 *
 *   b      JSBool          Boolean
 *   c      uint16/jschar   ECMA uint16, Unicode char
 *   i      int32           ECMA int32
 *   u      uint32          ECMA uint32
 *   j      int32           Rounded int32 (coordinate)
 *   d      jsdouble        IEEE double
 *   I      jsdouble        Integral IEEE double
 *   s      char *          C string
 *   S      JSString *      Unicode string, accessed by a JSString pointer
 *   W      jschar *        Unicode character vector, 0-terminated (W for wide)
 *   o      JSObject *      Object reference
 *   f      JSFunction *    Function private
 *   F      jsval           Function as jsval (type checked, not converted)
 *   v      jsval           Argument value (no conversion)
 *   *      N/A             Skip this argument (no vararg)
 *   /      N/A             End of required arguments
 *
		if(cx == 'b')
		{
			if(!JS_ValueToBoolean(cx, argv[i], va_arg(args, JSBool*)))
			{
				sprintf(msg, error_msg, i, "Boolean");
				JS_SetPendingException(cx, STRING_TO_JSVAL(msg));
				return JS_FALSE;
			}
		}
		else if(cx == 'i')
		{
			if(!JS_ValueToECMAInt32(cx, argv[i], va_arg(args, int32*)))
			{
				sprintf(msg, error_msg, i, "Integer");
				JS_SetPendingException(cx, STRING_TO_JSVAL(msg));
				return JS_FALSE;
			}
		}
		else if(cx == 'u')
		{
			if(!JS_ValueToECMAUint32(cx, argv[i], va_arg(args, uint32*)))
			{
				sprintf(msg, error_msg, i, "Integer");
				JS_SetPendingException(cx, STRING_TO_JSVAL(msg));
				return JS_FALSE;
			}
		}
		else if(cx == 'd')
	}*/
	return JS_TRUE;
}
