#include <windows.h>

#include "JSGlobalFuncs.h"
#include "prthread.h"
#include "helpers.h"
#include "Script.h"

JSAPI_FUNC(global_log)
{
	for(uintN i = 0; i < argc; i++)
	{
		char* msg = JSVAL_TO_STR(cx, argv[i]);
		// TODO: Add a function to replace % with %%
		Log(msg);
	}
	return JS_TRUE;
}

JSAPI_FUNC(global_sleep)
{
	if(JSVAL_IS_INT(argv[0]))
	{
		PR_Sleep(JSVAL_TO_INT(argv[0]));
	}
	return JS_TRUE;
}

JSAPI_FUNC(core_abort)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	script->Abort();
	return JS_TRUE;
}

JSAPI_FUNC(core_abortAll)
{
	Script::AbortAll();
	return JS_TRUE;
}

JSAPI_FUNC(script_include)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(script->Include(JSVAL_TO_STR(cx, argv[0])));
	return JS_TRUE;
}

JSAPI_FUNC(script_isIncluded)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(script->IsIncluded(JSVAL_TO_STR(cx, argv[0])));
	return JS_TRUE;
}

JSAPI_FUNC(script_addIncludePath)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	script->AddIncludePath(JSVAL_TO_STR(cx, argv[0]));
	return JS_TRUE;
}

JSAPI_FUNC(core_addEvent)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	script->AddEvent(JSVAL_TO_STR(cx, argv[0]), argv[1]);	
	return JS_TRUE;
}

JSAPI_FUNC(core_removeEvent)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	script->RemoveEvent(JSVAL_TO_STR(cx, argv[0]), argv[1]);	
	return JS_TRUE;
}

JSAPI_FUNC(core_clearEvent)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	script->ClearEvent(JSVAL_TO_STR(cx, argv[0]));	
	return JS_TRUE;
}

JSAPI_FUNC(core_clearAllEvents)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	script->ClearAllEvents();	
	return JS_TRUE;
}
