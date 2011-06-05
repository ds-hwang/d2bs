#include <process.h>
#include <algorithm>
#include <iterator>

#include "AutoLock.hpp"
#include "Engine.hpp"
#include "Script.hpp"
#include "JSClasses.hpp"

namespace Core {

Script::Script(const wchar_t* path, Engine* engine) :
	engine(engine), cx(nullptr), script(nullptr), obj(nullptr), paths(nullptr),
	state(Failed), oldState(Failed), thread(INVALID_HANDLE_VALUE), pause(INVALID_HANDLE_VALUE)
{
	InitializeCriticalSection(&lock);
	AutoLock l(&lock);
	pause = CreateEvent(nullptr, false, false, nullptr);

	cx = engine->GetContext();
	JSAutoRequest req(cx);

	JS_SetContextPrivate(cx, this);
	obj = JS_NewCompartmentAndGlobalObject(cx, &global, nullptr);

	JSAutoEnterCompartment comp;
	comp.enter(cx, obj);

	paths = JS_NewArrayObject(cx, 0, nullptr);
	JS_AddObjectRoot(cx, &paths);

	engine->InitClasses(this);

	static char* funcs[] = {"require", "load"};
	for(int i = 0; i < (sizeof(funcs) / sizeof(char*)); i++)
	{
		jsval func = JSVAL_VOID;
		JS_GetProperty(cx, obj, funcs[i], &func);
		JS_DefineProperty(cx, JSVAL_TO_OBJECT(func), "paths", OBJECT_TO_JSVAL(paths), nullptr, nullptr, JSPROP_STATIC);
	}

	std::string cpath;
	std::wstring wpath(path);
	cpath.assign(wpath.begin(), wpath.end());

	script = JS_CompileFile(cx, obj, cpath.c_str());
	if(script != nullptr) oldState = state = Waiting;
}
Script::~Script(void)
{
	AutoLock l(&lock);
	JS_RemoveObjectRoot(cx, &paths);

	RemoveListeners();

	engine->ReleaseContext(cx);
}

void Script::Start(void)
{
	if(state == Waiting)
		thread = (HANDLE)_beginthread(Script::ThreadProc, 0, this);
}
void Script::Stop(void)
{
	state = Stopping;
	JS_TriggerOperationCallback(cx);
}
void Script::Pause(void)
{
	if(state == Running)
	{
		AutoLock l(&lock);
		oldState = state;
		state = Paused;
		ResetEvent(pause);
		JS_TriggerOperationCallback(cx);
	}
}
void Script::Resume(void)
{
	if(state == Paused)
	{
		AutoLock l(&lock);
		state = oldState;
		SetEvent(pause);
	}
}

void Script::AddListener(const char* evt, jsval callback)
{
	AutoLock l(&lock);

	map[evt].push_back(callback);
	JS_AddValueRoot(cx, &(map[evt].back()));
}

void Script::RemoveListener(const char* evt, jsval callback)
{
	AutoLock l(&lock);
	JSAutoRequest req(cx);

	jsval* ptr = nullptr;
	for(auto it = map[evt].begin(); it != map[evt].end(); it++)
		if((*it == callback)) ptr = &(*it);
	if(ptr != nullptr)
	{
		JS_RemoveValueRoot(cx, ptr);
		map[evt].remove(*ptr);
	}
}
void Script::RemoveListeners(const char* evt)
{
	AutoLock l(&lock);
	JSAutoRequest req(cx);

	for(auto it = map[evt].begin(); it != map[evt].end(); it++)
		JS_RemoveValueRoot(cx, &(*it));
	map[evt].clear();
}
void Script::RemoveListeners()
{
	AutoLock l(&lock);
	JSAutoRequest req(cx);

	for(auto evt = map.begin(); evt != map.end(); evt++)
		for(auto it = evt->second.begin(); it != evt->second.end(); it++)
			JS_RemoveValueRoot(cx, &(*it));
	map.clear();
}

void Script::FireEvent(const char* evt, std::list<JS::Anchor<jsval>> args)
{
}

JSBool Script::OperationCallback(JSContext *cx)
{
	Script* self = (Script*)JS_GetContextPrivate(cx);

	switch(self->state)
	{
		case Paused: {
			JSAutoSuspendRequest req(cx);
			WaitForSingleObject(self->pause, INFINITE);
			break;
		}
		case Stopping: {
			// remove any handlers, set the state, etc.
			// NOTE: for now, wait for the thread to exit; later, wait a given length of
			// time after throwing an exception on the script and then kill it and shut down
			JSAutoSuspendRequest req(cx);
			WaitForSingleObject(self->thread, INFINITE);
			self->state = Stopped;
			break;
		}
		default: /* do nothing */ break;
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
	if(JS_ExecuteScript(self->cx, self->obj, self->script, &dummy))
	{
		jsval main = JSVAL_NULL;
		if(JS_GetProperty(self->cx, self->obj, "main", &main) && !JSVAL_IS_VOID(main) &&
		   JS_ObjectIsFunction(self->cx, JSVAL_TO_OBJECT(main)))
		{
			jsval dummy;
			JS::Call(self->cx, self->obj, main, 0, nullptr, &dummy);
		}
	}

	self->state = Done;
}

}
