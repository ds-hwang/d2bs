#include "JSScript.h"
#include "Script.h"

#include "debugnew/debug_new.h"

typedef struct { ScriptMap::iterator it; } ScriptIterator;

VOID script_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("script finalize");

	ScriptIterator* ptr = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	if(ptr)
		delete ptr;
}

JSAPI_PROP(script_getProperty)
{
	CDebug cDbg("script getProperty");

	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);

	if(!script)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
	{
		case SCRIPT_FILENAME:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, script->it->second->GetFilename()));
			break;
		case SCRIPT_GAMETYPE:
			*vp = script->it->second->GetState() == InGame ? INT_TO_JSVAL(0) : INT_TO_JSVAL(1);
			break;
		case SCRIPT_RUNNING:
			*vp = BOOLEAN_TO_JSVAL(script->it->second->IsRunning());
			break;
		case SCRIPT_THREADID:
			*vp = INT_TO_JSVAL(script->it->second->GetThreadId());
			break;
		default:
			break;
	}

	return JS_TRUE;
}

JSAPI_FUNC(script_getNext)
{
	CDebug cDbg("script getNext");

	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	if(script->it != Script::GetLastScript())
	{
		script->it++;
		*rval = BOOLEAN_TO_JSVAL(TRUE);
	}

	return JS_TRUE;
}

JSAPI_FUNC(script_stop)
{
	CDebug cDbg("script stop");

	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	script->it->second->Stop();

	return JS_TRUE;
}

JSAPI_FUNC(script_pause)
{
	CDebug cDbg("script pause");
	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	script->it->second->Pause();
	return JS_TRUE;
}

JSAPI_FUNC(script_resume)
{
	CDebug cDbg("script resume");
	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	script->it->second->Resume();
	return JS_TRUE;	
}

JSAPI_FUNC(script_send)
{
	CDebug cDbg("script send");

	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);

	AutoRoot** args = new AutoRoot*[1];
	args[0] = new AutoRoot(cx, argv[0]);

	// this event has to occur as such because it's not a broadcasted event, just a local one
	script->it->second->ExecEventAsync("scriptmsg", 1, args);

	return JS_TRUE;
}

JSAPI_FUNC(my_getScript)
{
	CDebug cDbg("getScript");

	ScriptIterator* ptr = new ScriptIterator();
	ptr->it = Script::GetFirstScript();
	JSObject* res = BuildObject(cx, &script_class, script_methods, script_props, ptr);
	if(!res)
		THROW_ERROR(cx, obj, "Failed to build the script object");
	*rval = OBJECT_TO_JSVAL(res);

	return JS_TRUE;
}
