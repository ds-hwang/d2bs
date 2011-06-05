#include <process.h>
#include <algorithm>
#include <iterator>

#include "AutoLock.hpp"
#include "Engine.hpp"
#include "Script.hpp"
#include "JSClasses.hpp"

namespace Core {

Script::Script(const wchar_t* path, Engine* engine) :
	engine(engine), cx(nullptr), script(nullptr), obj(nullptr), paths(nullptr), name(path),
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
		thread = (HANDLE)_beginthread(Script::MainProc, 0, this);
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

void Script::AddListener(const wchar_t* evt, jsval callback)
{
	AutoLock l(&lock);

	map[evt].push_back(callback);
	JS_AddValueRoot(cx, &(map[evt].back()));
}

void Script::RemoveListener(const wchar_t* evt, jsval callback)
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
void Script::RemoveListeners(const wchar_t* evt)
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

void Script::FireEvent(Event* evt)
{
	ScriptEvent* evtData = new ScriptEvent;
	evtData->evt = evt;
	evtData->who = this;
	QueueUserWorkItem(Script::EventProc, evtData, WT_EXECUTEDEFAULT);
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
			// wait 1 second after throwing an exception on the script and then kill it
			self->RemoveListeners();

			JS_ReportError(cx, "Aborted");
			JS_ReportPendingException(cx);

			JSAutoSuspendRequest req(cx);
			WaitForSingleObject(self->thread, 1000);
			TerminateThread(self->thread, 0);
			self->thread = INVALID_HANDLE_VALUE;

			self->state = Stopped;
			break;
		}
		default: /* do nothing */ break;
	}

	return JS_TRUE;
}

void __cdecl Script::MainProc(void* args)
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

DWORD WINAPI Script::EventProc(void* args)
{
	ScriptEvent* se = (ScriptEvent*)args;
	Event* evt = se->evt;
	Script* who = se->who;

	std::list<jsval> funcs = who->map[evt->evtName];

	int len = evt->args.size();
	jsval* arg = new jsval[len];

	auto end = evt->args.end();
	uint32 i = 0;
	for(auto it = evt->args.begin(); it != end; it++)
	{
		if(it->type() == typeid(int)) {
			int x = boost::any_cast<int>(*it);
			arg[i++] = INT_TO_JSVAL(x);
		} else if(it->type() == typeid(double)) {
			double x = boost::any_cast<double>(*it);
			arg[i++] = DOUBLE_TO_JSVAL(x);
		} else if(it->type() == typeid(boost::shared_ptr<JSAutoRoot>)) {
			boost::shared_ptr<JSAutoRoot> x = boost::any_cast<boost::shared_ptr<JSAutoRoot>>(*it);
			arg[i++] = *(jsval*)x.get()->get();
		} else if(it->type() == typeid(boost::shared_array<char>)) {
			boost::shared_array<char> x = boost::any_cast<boost::shared_array<char>>(*it);
			arg[i++] = STRING_TO_JSVAL(JS_NewStringCopyZ(who->cx, x.get()));
		}
	}

	for(auto it = funcs.begin(); it != funcs.end(); it++)
	{
		JSAutoRequest req(who->cx);
		JSAutoEnterCompartment comp;
		comp.enter(who->cx, who->obj);

		jsval result;
		JS::Call(who->cx, who->obj, *it, len, arg, &result);
		if(JSVAL_IS_BOOLEAN(result) && JSVAL_TO_BOOLEAN(result) == JS_TRUE)
			evt->result = true;
	}

	if(evt->blocker != INVALID_HANDLE_VALUE)
		SetEvent(evt->blocker);

	delete se;
	// if there's a blocker, then the event source is waiting on the result, so don't delete it
	if(evt->blocker != INVALID_HANDLE_VALUE)
		delete evt;
	return 0;
}

}
