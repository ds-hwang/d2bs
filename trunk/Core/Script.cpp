#include <process.h>
#include <algorithm>
#include <iterator>

#include "Engine.hpp"
#include "Script.hpp"
#include "JSClasses.hpp"

Script::Script(const wchar_t* path, Engine* engine) :
	engine(engine), cx(nullptr), script(nullptr), obj(nullptr), paths(nullptr),
	state(Failed), thread(INVALID_HANDLE_VALUE)
{
	InitializeCriticalSection(&lock);

	cx = engine->GetContext();
	JSAutoRequest req(cx);

	JS_SetContextPrivate(cx, this);
	obj = JS_NewCompartmentAndGlobalObject(cx, &global, nullptr);

	JSAutoEnterCompartment comp;
	comp.enter(cx, obj);

	paths = JS_NewArrayObject(cx, 0, nullptr);
	JS_AddObjectRoot(cx, &paths);

	engine->InitClasses(this);

	jsval require = JSVAL_VOID, load = JSVAL_VOID;
	JS_GetProperty(cx, obj, "require", &require);
	JS_GetProperty(cx, obj, "load", &load);
	JS_DefineProperty(cx, JSVAL_TO_OBJECT(require), "paths", OBJECT_TO_JSVAL(paths), nullptr, nullptr, JSPROP_STATIC);
	JS_DefineProperty(cx, JSVAL_TO_OBJECT(load), "paths", OBJECT_TO_JSVAL(paths), nullptr, nullptr, JSPROP_STATIC);

	std::string cpath;
	std::wstring wpath(path);
	cpath.assign(wpath.begin(), wpath.end());

	script = JS_CompileFile(cx, obj, cpath.c_str());
	if(script != nullptr) state = Waiting;
}
Script::~Script(void)
{
	JS_RemoveObjectRoot(cx, &paths);
	engine->ReleaseContext(cx);
}

void Script::Start(void)
{
	thread = (HANDLE)_beginthread(Script::ThreadProc, 0, this);
}
void Script::Stop(void)
{
	state = Stopping;
	JS_TriggerOperationCallback(cx);
}
void Script::Pause(void)
{
	state = Paused;
	JS_TriggerOperationCallback(cx);
}

void Script::AddListener(const char* evt, jsval callback)
{
	EnterCriticalSection(&lock);
	map[evt].push_back(callback);
	JS_AddValueRoot(cx, &(map[evt].back()));
	LeaveCriticalSection(&lock);
}

void Script::RemoveListener(const char* evt, jsval callback)
{
	EnterCriticalSection(&lock);
	jsval* ptr = nullptr;
	for(auto it = map[evt].begin(); it != map[evt].end(); it++)
		if((*it == callback)) ptr = &(*it);
	if(ptr != nullptr)
	{
		JS_RemoveValueRoot(cx, ptr);
		map[evt].remove(*ptr);
	}
	LeaveCriticalSection(&lock);
}

void Script::RemoveListener(const char* evt)
{
	EnterCriticalSection(&lock);
	for(auto it = map[evt].begin(); it != map[evt].end(); it++)
		JS_RemoveValueRoot(cx, &(*it));
	map[evt].clear();
	LeaveCriticalSection(&lock);
}

void Script::RemoveListeners()
{
	EnterCriticalSection(&lock);
	for(auto evt = map.begin(); evt != map.end(); evt++)
		for(auto it = evt->second.begin(); it != evt->second.end(); it++)
			JS_RemoveValueRoot(cx, &(*it));
	map.clear();
	LeaveCriticalSection(&lock);
}

void Script::FireEvent(const char* evt, std::list<JS::Anchor<jsval>> args)
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
	if(self->state == Failed)
		return;

	JSAutoRequest req(self->cx);
	JSAutoEnterCompartment comp;
	comp.enter(self->cx, self->obj);

	self->state = Running;

	jsval dummy;
	JS_ExecuteScript(self->cx, self->obj, self->script, &dummy);

	jsval main = JSVAL_NULL;
	if(JS_GetProperty(self->cx, self->obj, "main", &main) && !JSVAL_IS_VOID(main) &&
	   JS_ObjectIsFunction(self->cx, JSVAL_TO_OBJECT(main)))
	{
		jsval dummy;
		JS::Call(self->cx, self->obj, main, 0, nullptr, &dummy);
	}

	self->state = Done;
}
