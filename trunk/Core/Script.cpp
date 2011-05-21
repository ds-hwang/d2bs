#include <process.h>

#include "ScriptEngine.hpp"
#include "Script.hpp"
#include "JSClasses.hpp"

EXPORT Script::Script(const char* path, ScriptEngine* engine)
{
	cx = engine->GetContext();
	obj = JS_NewCompartmentAndGlobalObject(cx, &global, NULL);

	JS_InitStandardClasses(cx, obj);
	engine->InitClasses(this);

	script = JS_CompileFile(cx, obj, path);
	state = Waiting;
}

EXPORT void Script::Start(void)
{
	thread = (HANDLE)_beginthread(Script::ThreadProc, 0, this);
}

EXPORT void Script::Stop(void)
{
	state = Stopping;
	JS_TriggerOperationCallback(cx);
}

JSBool Script::OperationCallback(JSContext *cx)
{
	Script* self = (Script*)JS_GetContextPrivate(cx);

	if(self->state == Stopping)
	{
		// remove any handlers, set the state, etc.
		// NB: for now, wait for the thread to exit; later, wait a given length of
		// time after throwing an exception on the script and then kill it and shut down
		WaitForSingleObject(self->thread, INFINITE);
		self->state = Stopped;
	}

	return JS_TRUE;
}

void __cdecl Script::ThreadProc(void* args)
{
	Script* self = (Script*)args;

	self->state = Running;

	JS_ExecuteScript(self->cx, self->obj, self->script, NULL);

	jsval main = JSVAL_NULL;
	if(JS_GetProperty(self->cx, self->obj, "main", &main) &&
	   JS_ObjectIsFunction(self->cx, JSVAL_TO_OBJECT(main)))
	{
		jsval dummy;
		JS::Call(self->cx, self->obj, main, 0, NULL, &dummy);
	}

	self->state = Done;
}
