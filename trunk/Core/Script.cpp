#include <process.h>
#include <algorithm>
#include <iterator>

#include "ScriptEngine.hpp"
#include "Script.hpp"
#include "JSClasses.hpp"

EXPORT Script::Script(const wchar_t* path, ScriptEngine* engine)
{
	InitializeCriticalSection(&lock);

	cx = engine->GetContext();
	JS_SetContextPrivate(cx, this);
	obj = JS_NewCompartmentAndGlobalObject(cx, &global, NULL);

	JS_InitStandardClasses(cx, obj);
	engine->InitClasses(this);

	std::string cpath;
	std::wstring wpath(path);
	cpath.assign(wpath.begin(), wpath.end());
	script = JS_CompileFile(cx, obj, cpath.c_str());
	state = Waiting;
	this->engine = engine;
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

EXPORT void Script::AddListener(const char* evt, jsval callback)
{
	EnterCriticalSection(&lock);
	map[evt].push_back(callback);
	JS_AddValueRoot(cx, &(map[evt].back()));
	LeaveCriticalSection(&lock);
}

EXPORT void Script::RemoveListener(const char* evt, jsval callback)
{
	EnterCriticalSection(&lock);
	jsval* ptr = NULL;
	for(auto it = map[evt].begin(); it != map[evt].end(); it++)
		if((*it == callback)) ptr = &(*it);
	if(ptr != NULL)
	{
		JS_RemoveValueRoot(cx, ptr);
		map[evt].remove(*ptr);
	}
	LeaveCriticalSection(&lock);
}

EXPORT void Script::RemoveListener(const char* evt)
{
	EnterCriticalSection(&lock);
	for(auto it = map[evt].begin(); it != map[evt].end(); it++)
		JS_RemoveValueRoot(cx, &(*it));
	map[evt].clear();
	LeaveCriticalSection(&lock);
}

EXPORT void Script::RemoveListeners()
{
	EnterCriticalSection(&lock);
	for(auto evt = map.begin(); evt != map.end(); evt++)
		for(auto it = evt->second.begin(); it != evt->second.end(); it++)
			JS_RemoveValueRoot(cx, &(*it));
	map.clear();
	LeaveCriticalSection(&lock);
}

EXPORT void Script::FireEvent(const char* evt, std::list<JS::Anchor<jsval>> args)
{
}

JSBool Script::OperationCallback(JSContext *cx)
{
	Script* self = (Script*)JS_GetContextPrivate(cx);

	// check the event queue or something

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

	jsval dummy;
	JS_ExecuteScript(self->cx, self->obj, self->script, &dummy);

	jsval main = JSVAL_NULL;
	if(JS_GetProperty(self->cx, self->obj, "main", &main) &&
	   JS_ObjectIsFunction(self->cx, JSVAL_TO_OBJECT(main)))
	{
		jsval dummy;
		JS::Call(self->cx, self->obj, main, 0, NULL, &dummy);
	}

	self->state = Done;
}
