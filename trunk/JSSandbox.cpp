#include "JSSandbox.h"
#include "ScriptEngine.h"
#include "CDebug.h"

#include "debugnew/debug_new.h"

JSBool sandbox_ctor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sandbox ctor");
	sandbox* box = new sandbox; // leaked?
//Oh here there
	box->context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
//Oh here there
	box->innerObj = JS_NewObject(box->context, &global_obj, NULL, NULL);
	if(!box->innerObj)
	{
//Oh here there
		JS_DestroyContext(box->context);
		delete box;
		return JS_TRUE;
	}
//Oh here there
	JS_InitStandardClasses(box->context, box->innerObj);
	// TODO: add a default include function for sandboxed scripts
	// how do I do that individually though? :/

//Oh here there
	JSObject* res = JS_NewObject(cx, &sandbox_class, NULL, NULL);
//Oh here there
	JS_AddRoot(cx, &res);
//Oh here there
	if(!res || !JS_DefineFunctions(cx, res, sandbox_methods))
	{
//Oh here there
		JS_RemoveRoot(box->context, &box->innerObj);
//Oh here there
		JS_DestroyContext(box->context);
		delete box;
//Oh here there
		return JS_FALSE;
	}
//Oh here there
	JS_SetPrivate(cx, res, box);
//Oh here there
	JS_RemoveRoot(cx, &res);
	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}

JSBool sandbox_addProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("sandbox addProperty");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	JSContext* cxptr = (!box ? cx : box->context);
	JSObject* ptr = (!box ? obj : box->innerObj);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
//Oh here there
		JS_ValueToInt32(cx, id, &i);
		char name[32];
		_itoa_s(i, name, 32, 10);
		JSBool found;
//Oh here there
		JS_HasProperty(cxptr, ptr, name, &found);
		if(found)
			return JS_TRUE;

//Oh here there
		return JS_DefineProperty(cxptr, ptr, name, *vp, NULL, NULL, JSPROP_ENUMERATE);
	}
	else if(JSVAL_IS_STRING(id))
	{
//Oh here there
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		JSBool found;
//Oh here there
		JS_HasProperty(cxptr, ptr, name, &found);
		if(found)
			return JS_TRUE;

//Oh here there
		return JS_DefineProperty(cxptr, ptr, name, *vp, NULL, NULL, JSPROP_ENUMERATE);
	}
//Oh here there
	return JS_FALSE;
}

JSBool sandbox_delProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("sandbox delProperty");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
//Oh here there
		JS_ValueToInt32(cx, id, &i);
		char name[32];
		_itoa_s(i, name, 32, 10);
		if(box)
//Oh here there
			JS_DeleteProperty(box->context, box->innerObj, name);
		return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(id))
	{
//Oh here there
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		if(box)
//Oh here there
			JS_DeleteProperty(box->context, box->innerObj, name);
		return JS_TRUE;
	}
//Oh here there
	return JS_FALSE;
}

JSBool sandbox_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("sandbox getProperty");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
//Oh here there
		JS_ValueToInt32(cx, id, &i);
		char name[32];
		_itoa_s(i, name, 32, 10);
		*vp = JSVAL_VOID;
		if(box)
//Oh here there
			JS_LookupProperty(box->context, box->innerObj, name, vp);
		if(JSVAL_IS_VOID(*vp))
//Oh here there
			JS_LookupProperty(cx, obj, name, vp);
		return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(id))
	{
//Oh here there
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		*vp = JSVAL_VOID;
		if(box)
//Oh here there
			JS_LookupProperty(box->context, box->innerObj, name, vp);
		if(JSVAL_IS_VOID(*vp))
//Oh here there
			JS_LookupProperty(cx, obj, name, vp);
		return JS_TRUE;
	}
//Oh here there
	return JS_FALSE;
}

JSBool sandbox_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	CDebug cDbg("sandbox setProperty");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);

	if(JSVAL_IS_INT(id))
	{
		int32 i;
//Oh here there
		JS_ValueToInt32(cx, id, &i);
		char name[32];
		_itoa_s(i, name, 32, 10);
		if(box)
//Oh here there
			JS_SetProperty(box->context, box->innerObj, name, vp);
		return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(id))
	{
//Oh here there
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(id));
		if(box)
//Oh here there
			JS_SetProperty(box->context, box->innerObj, name, vp);
		return JS_TRUE;
	}
//Oh here there
	return JS_FALSE;
}

void sandbox_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("sandbox finalize");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	if(box) {
//Oh here there
		JS_DestroyContext(box->context);
		delete box;
	}
}

JSBool sandbox_eval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sandbox eval");
	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
//Oh here there
		sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
		if(!box)
			THROW_ERROR(cx, obj, "Invalid execution object!");
//Oh here there
		char* code = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		jsval result;
//Oh here there
		if(JS_BufferIsCompilableUnit(box->context, box->innerObj, code, strlen(code)) &&
//Oh here there
			JS_EvaluateScript(box->context, box->innerObj, code, strlen(code), "sandbox", 0, &result))
				*rval = result;
	} else THROW_ERROR(cx, obj, "Invalid parameter, string expected");
	return JS_TRUE;
}

JSBool sandbox_include(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sandbox include");
	*rval = JSVAL_FALSE;
	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
//Oh here there
		sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
//Oh here there
		char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		if(file && strlen(file) <= _MAX_FNAME && box)
		{
			char buf[_MAX_PATH+_MAX_FNAME];
			sprintf(buf, "%s\\libs\\%s", Vars.szScriptPath, file);
			if(box->list.count(std::string(file)) == -1)
			{
//Oh here there
				JSScript* tmp = JS_CompileFile(box->context, box->innerObj, buf);
				if(tmp)
				{
					jsval result;
//Oh here there
					if(JS_ExecuteScript(box->context, box->innerObj, tmp, &result))
					{
						box->list[file] = true;
						*rval = result;
					}
//Oh here there
					JS_DestroyScript(cx, tmp);
				}
			}
		}
	} else THROW_ERROR(cx, obj, "Invalid parameter, file expected");
	return JS_TRUE;
}

JSBool sandbox_isIncluded(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sandbox isIncluded");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	if(argc > 0 && JSVAL_IS_STRING(argv[0]) && box)
	{
//Oh here there
		char* file = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		char buf[_MAX_PATH+_MAX_FNAME];
		sprintf(buf, "%s\\libs\\%s", Vars.szScriptPath, file);
		*rval = BOOLEAN_TO_JSVAL(!!box->list.count(std::string(buf)));
	} else THROW_ERROR(cx, obj, "Invalid parameter, file expected");
	return JS_TRUE;
}

JSBool sandbox_clear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("sandbox clear");
//Oh here there
	sandbox* box = (sandbox*)JS_GetInstancePrivate(cx, obj, &sandbox_class, NULL);
	if(box)
//Oh here there
		JS_ClearScope(cx, box->innerObj);
	return JS_TRUE;
}