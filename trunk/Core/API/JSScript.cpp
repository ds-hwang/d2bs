#include <utility>

#include "JSScript.h"
#include "Script.h"
#include "ScriptEngine.h"
#include "D2BS.h"
#include "Helpers.h"

EMPTY_CTOR(script)


JSAPI_PROP(script_getProperty)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	// TODO: make this check stronger
	if(!script)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case SCRIPT_FILENAME:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, script->GetShortFilename().c_str()));
			break;
		case SCRIPT_RUNNING:
			*vp = BOOLEAN_TO_JSVAL(script->GetExecState() == ScriptStateRunning);
			break;
		case SCRIPT_THREADID:
			*vp = INT_TO_JSVAL(script->GetThreadId());
			break;
		default:
			break;
	}

	return JS_TRUE;
}

JSAPI_FUNC(script_getNext)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	if(JS_ContextIterator(ScriptEngine::GetRuntime(), &iterp) == NULL || !JS_GetContextPrivate(iterp))
		*rval = JSVAL_FALSE;
	else
	{
		JS_SetPrivate(cx, obj, iterp);
		*rval = JSVAL_TRUE;
	}

	return JS_TRUE;
}

JSAPI_FUNC(script_stop)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	if(script->GetExecState() == ScriptStateRunning || script->GetExecState() == ScriptStatePaused)
		script->Stop();

	return JS_TRUE;
}

JSAPI_FUNC(script_pause)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	if(script->GetExecState() == ScriptStateRunning)
		script->Pause();

	return JS_TRUE;
}

JSAPI_FUNC(script_resume)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);

	if(script->GetExecState() == ScriptStatePaused)
		script->Resume();

	return JS_TRUE;
}

JSAPI_FUNC(script_send)
{
	JSContext* iterp = (JSContext*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	Script* script = (Script*)JS_GetContextPrivate(iterp);
	if(argc < 1)
		THROW_ERROR(cx, "You must specify at least one thing to send to the other script");

	// this event has to occur as such because it's not a broadcasted event, just a local one
	char* format = new char[argc+1];
	format[argc] = '\0';
	ArgList* args = new ArgList;

	for(uintN i = 0; i < argc; i++)
	{
		format[i] = 'v';
		args->push_back(std::make_pair((QWORD)argv[i], JSVal));
	}

	script->ExecEventAsync("scriptmsg", format, args);

	return JS_TRUE;
}

JSAPI_FUNC(my_getScript)
{
	JSContext* iterp = NULL;
	if(argc == 1 && JSVAL_IS_BOOLEAN(argv[0]) && JSVAL_TO_BOOLEAN(argv[0]) == JS_TRUE)
		iterp = cx;
	else if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		// loop over the Scripts in ScriptEngine and find the one with the right threadid
		DWORD tid = (DWORD)JSVAL_TO_INT(argv[0]);
		Script* script = ScriptEngine::FindByTid(tid);
		if(script != NULL)
			iterp = script->GetContext();
		else
			return JS_TRUE;
	}
	else if(argc == 1 && JSVAL_IS_STRING(argv[0]))
	{
		char* name = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));

		Script* script = ScriptEngine::FindByName(name);
		if(script != NULL)
			iterp = script->GetContext();
		else
			return JS_TRUE;
	}
	else
	{
		if(!JS_ContextIterator(ScriptEngine::GetRuntime(), &iterp))
			return JS_TRUE;
	}

	JSObject* res = BuildObject(cx, &script_class, script_methods, script_props, iterp);
	if(!res)
		THROW_ERROR(cx, "Failed to build the script object");

	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}
