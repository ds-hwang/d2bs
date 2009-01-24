#include "JSScript.h"
#include "Script.h"

typedef struct { ScriptMap::iterator it; } ScriptIterator;

VOID script_finalize(JSContext *cx, JSObject *obj)
{
	CDebug cDbg("script finalize");

	ScriptIterator* ptr = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	if(ptr)
		delete ptr;
}

INT script_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
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
		default:
			break;
	}

	return JS_TRUE;
}

INT script_getNext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
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

INT script_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("script stop");

	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);
	script->it->second->Stop();

	return JS_TRUE;
}

INT script_send(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	CDebug cDbg("script send");

	ScriptIterator* script = (ScriptIterator*)JS_GetInstancePrivate(cx, obj, &script_class, NULL);

	AutoRoot** args = new AutoRoot*[1];
	args[0] = new AutoRoot(cx, argv[0]);

	// this event has to occur as such because it's not a broadcasted event, just a local one
	script->it->second->ExecEventAsync("scriptmsg", 1, args);

	return JS_TRUE;
}

JSBool my_getScript(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
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
