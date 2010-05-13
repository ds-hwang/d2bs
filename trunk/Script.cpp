#include <io.h>
#include <string>
#include <cctype> // tolower
#include <algorithm>
#include <process.h>

#include "D2Ptrs.h"

#include "Script.h"

#include "Core.h"
#include "Helpers.h"
#include "ScriptEngine.h"
#include "D2BS.h"

using namespace std;

Script::Script(string file) :
		fileName(""), context(NULL), globalObject(NULL), scriptObject(NULL),
		script(NULL), execCount(0),
		scriptExecState(ScriptStateCreation), threadHandle(INVALID_HANDLE_VALUE), threadId(0)
{
	InitializeCriticalSection(&lock);
	EnterCriticalSection(&lock);

	if(!!_access(file.c_str(), 0))
		throw std::exception("File not found");

	fileName = file;
	std::transform(fileName.begin(), fileName.end(), fileName.begin(), tolower);
	replace(fileName.begin(), fileName.end(), '/', '\\');

	try
	{
		context = ScriptEngine::AcquireContext();
		if(!context)
			throw std::exception("Couldn't create the context");
	}
	catch(std::exception &)
	{
		LeaveCriticalSection(&lock);
		throw;
	}

	try
	{
		JS_SetContextPrivate(context, this);
		JS_BeginRequest(context);

		globalObject = JS_GetGlobalObject(context);

		script = JS_CompileFile(context, globalObject, fileName.c_str());

		if(!script)
			throw std::exception("Couldn't compile the script");
	}
	catch(std::exception &)
	{
		JS_EndRequest(context);
		ScriptEngine::ReleaseContext(context);

		LeaveCriticalSection(&lock);
		throw;
	}

	try
	{
		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Couldn't create the script object");

		if(JS_AddNamedRoot(context, &scriptObject, "script object") == JS_FALSE)
			throw std::exception("Couldn't add named root for scriptObject");

		JS_EndRequest(context);
		JS_ClearContextThread(context);

		LeaveCriticalSection(&lock);
	}
	catch(std::exception&)
	{
		if(scriptObject)
		{
			JS_RemoveRoot(&scriptObject);
		}
		else
			JS_DestroyScript(context, script);
 
		JS_EndRequest(context);
		ScriptEngine::ReleaseContext(context);

		LeaveCriticalSection(&lock);
		throw;
	}
}

Script::~Script(void)
{
	EnterCriticalSection(&lock);

	ASSERT(scriptExecState == ScriptStateAborted);
	ASSERT(threadHandle == INVALID_HANDLE_VALUE);

	if(!JS_GetContextThread(context))
		JS_SetContextThread(context);

	JS_BeginRequest(context);

	// these calls can, and probably should, be moved to the context callback on cleanup
	JS_RemoveRoot(&globalObject);
	JS_RemoveRoot(&scriptObject);

	JS_EndRequest(context);
	ScriptEngine::ReleaseContext(context);

	includes.clear();

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;
	scriptExecState = ScriptStateUnknown;

	LeaveCriticalSection(&lock);
	DeleteCriticalSection(&lock);
}

int Script::GetExecutionCount(void)
{
	return execCount;
}

DWORD Script::GetThreadId(void)
{
	return (threadHandle == NULL ? -1 : threadId);
}

void Script::Run(void)
{
	// only let the script run if it's in initial unknown state
	ASSERT(scriptExecState == ScriptStateCreation);

	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	threadId = GetCurrentThreadId();

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;

	JS_SetContextThread(context);
	JS_BeginRequest(context);

	scriptExecState = ScriptStateRunning;

	if(JS_ExecuteScript(context, globalObject, script, &dummy) != JS_FALSE &&
	   JS_GetProperty(context, globalObject, "main", &main) != JS_FALSE &&
	   JSVAL_IS_FUNCTION(context, main))
		JS_CallFunctionValue(context, globalObject, main, 0, NULL, &dummy);

	JS_EndRequest(context);
	JS_ClearContextThread(context);

	execCount++;
	Stop();
}

ScriptExecState Script::GetExecState()
{
	// This is a bit hacky, we can come up with a better fix, this is more
	// of a transitional workaround
	if(scriptExecState == ScriptStateRunning && 
			(!context || !JS_IsRunning(context)))
		scriptExecState = ScriptStateAborting;
	return scriptExecState;
}

void Script::Pause(void)
{
	//EnterCriticalSection(&lock);
	if(GetExecState() == ScriptStateRunning)
	{
		scriptExecState = ScriptStatePaused;
		JS_TriggerOperationCallback(context);
	}
	//LeaveCriticalSection(&lock);
}

void Script::Resume(void)
{
	//EnterCriticalSection(&lock);
	if(GetExecState() == ScriptStatePaused)
	{
		scriptExecState = ScriptStateRunning;
		JS_TriggerOperationCallback(context);
	}
	//LeaveCriticalSection(&lock);
}

void Script::Stop(bool force)
{
	// if we've already stopped just return
	if(GetExecState() == ScriptStateAborted)
		return;

	EnterCriticalSection(&lock);

	// TODO FIXME Stop should probably take an unsigned arg where 0 means
	// immediately stopping aka the ex reallyForce
	// going with 10 and 30 for now.
#if 0
	// normal wait: 500ms, forced wait: 300ms, really forced wait: 100ms

	// if we pass the time frame, just ignore the wait because the thread 
	// will end forcefully anyway
	int maxCount = force ? 10 : 30;
	for(int i = 0; GetExecState() == ScriptStateRunning; i++)
	{
		JS_TriggerOperationCallback(context);
		Sleep(10);
		if(i >= maxCount)
			break;
	}
#endif

	scriptExecState = ScriptStateAborted;
	JS_TriggerOperationCallback(context);

	ClearAllEvents();

	assert(threadHandle != INVALID_HANDLE_VALUE);
	if(threadHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(threadHandle);
		threadHandle = INVALID_HANDLE_VALUE;
	}

	// FIXME should only be printing substr
	Print("Script %s ended", GetShortFilename().c_str());

	LeaveCriticalSection(&lock);
}

bool Script::IsIncluded(const std::string &file)
{
	std::string File = file;

	std::transform(File.begin(), File.end(), File.begin(), tolower);
	replace(File.begin(), File.end(), '/', '\\');

	return !!includes.count(File);
}

bool Script::Include(const std::string &file)
{
	std::string File = file;
	// since includes will happen on the same thread, locking here is acceptable
	EnterCriticalSection(&lock);

	std::transform(File.begin(), File.end(), File.begin(), tolower);
	replace(File.begin(), File.end(), '/', '\\');

	// FIXME
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(!!includes.count(File) || !!inProgress.count(File) || fileName == File)
	{
		LeaveCriticalSection(&lock);
		return true;
	}
	bool rval = false;

	JS_BeginRequest(GetContext());

	JSScript* script = JS_CompileFile(GetContext(), GetGlobalObject(), File.c_str());
	if(script)
	{
		JSObject* scriptObj = JS_NewScriptObject(GetContext(), script);
		JS_AddRoot(&scriptObj);
		jsval dummy;
		inProgress[File] = true;
		rval = !!JS_ExecuteScript(GetContext(), GetGlobalObject(), script, &dummy);
		if(rval)
			includes[File] = true;
		inProgress.erase(File);
		JS_RemoveRoot(&scriptObj);
	}

	JS_EndRequest(GetContext());
	LeaveCriticalSection(&lock);

	return rval;
}

bool Script::IsListenerRegistered(const char* evtName)
{
	return strlen(evtName) > 0 && functions.count(evtName) > 0;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	EnterCriticalSection(&lock);
	if(JSVAL_IS_FUNCTION(context, evtFunc) && strlen(evtName) > 0)
	{
		AutoRoot* root = new AutoRoot(evtFunc);
		functions[evtName].push_back(root);
	}
	LeaveCriticalSection(&lock);
}

bool Script::IsRegisteredEvent(const char* evtName, jsval evtFunc)
{
	// nothing can be registered under an empty name
	if(strlen(evtName) < 1)
		return false;

	// if there are no events registered under that name at all, then obviously there
	// can't be a specific one registered under that name
	if(functions.count(evtName) < 1)
		return false;

	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
		if((*it)->value() == evtFunc)
			return true;

	return false;
}

void Script::UnregisterEvent(const char* evtName, jsval evtFunc)
{
	if(strlen(evtName) < 1)
		return;

	EnterCriticalSection(&lock);
	AutoRoot* func = NULL;
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		if((*it)->value() == evtFunc)
		{
			func = *it;
			break;
		}
	}
	if(func != NULL)
	{
		functions[evtName].remove(func);
		func->Release();
		delete func;
	}
	LeaveCriticalSection(&lock);
}

void Script::ClearEvent(const char* evtName)
{
	EnterCriticalSection(&lock);
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		(*it)->Release();
		delete *it;
	}
	functions[evtName].clear();
	LeaveCriticalSection(&lock);
}

void Script::ClearAllEvents(void)
{
	EnterCriticalSection(&lock);
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
	functions.clear();
	LeaveCriticalSection(&lock);
}

// TODO: clean this up and re-build it to match SpawnEvent... or maybe call SpawnEvent directly
#if 0
void Script::ExecEvent(const char* evtName, const char* format, uintN argc, void* argv)
{
	if((!IsAborted() && !IsPaused() && functions.count(evtName)) &&
		IsRunning() && !(GetScriptType() == InGame && ClientState() != ClientStateInGame))
	{
		FunctionList funcs = functions[evtName];
		JSContext* cx = ScriptEngine::AcquireContext();
		JS_EnterLocalRootScope(cx);
		// build the arg list
		void* markp = NULL;
		jsval* argv = NULL;
		argv = JS_PushArguments(cx, &markp, format, argv);

		for(FunctionList::iterator it = funcs.begin(); it != funcs.end(); it++)
		{
			jsval dummy = JSVAL_VOID;
			JS_CallFunctionValue(cx, GetGlobalObject(), (*it)->value(), argc, argv, &dummy);
		}

		// and clean it up
		JS_PopArguments(cx, markp);
		JS_LeaveLocalRootScope(cx);
		ScriptEngine::ReleaseContext(cx);
	}
}
#endif

void Script::ExecEventAsync(const char* evtName, const char* format, ArgList* args)
{
	if(GetExecState() == ScriptStateRunning && IsListenerRegistered(evtName) && functions.count(evtName))
	{
		Event* evt = new Event;
		evt->owner = this;
		evt->object = globalObject;
		evt->functions = functions[evtName];
		strcpy_s(evt->format, 10, format);
		evt->args = args;

		SpawnEvent(evt);
	}
	else
	{
		// clean up args
		for(ArgList::iterator it = args->begin(); it != args->end(); it++)
		{
			if(it->second == JSVal)
				JS_RemoveRoot(&(it->first));
		}
		delete args;
	}
}

DWORD WINAPI ScriptThread(void* data)
{
	Script* script = (Script*)data;
	if(script)
	{
		script->Run();
		if(Vars.bDisableCache)
			ScriptEngine::DisposeScript(script);
	}
	return 0;
}

void SpawnEvent(Event* evt)
{
	ASSERT(evt);

	if(evt != NULL && evt->owner->GetExecState() == ScriptStateRunning)
	{
		JSContext* cx = ScriptEngine::AcquireContext();
		JS_SetContextPrivate(cx, evt->owner);
		JS_BeginRequest(cx);
		JS_EnterLocalRootScope(cx);

		// build the arg list
		uintN argc = evt->args->size();
		jsval* argv = new jsval[argc];
		int i = 0;
		for(ArgList::iterator it = evt->args->begin(); it != evt->args->end(); it++, i++)
		{
			switch(it->second)
			{
				case String:
					argv[i] = STRING_TO_JSVAL((JSString*)it->first);
					break;
				case SignedInt: case UnsignedInt: case Double: case UnsignedShort:
					JS_NewNumberValue(cx, (jsdouble)it->first, &argv[i]);
					break;
				case Boolean:
					argv[i] = BOOLEAN_TO_JSVAL((JSBool)it->first);
					break;
				case JSVal:
					argv[i] = (jsval)it->first;
					break;
			}
		}

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			jsval dummy = JSVAL_VOID;
			JS_CallFunctionValue(cx, evt->object, (*it)->value(), argc, argv, &dummy);
		}

		// and clean it up
		JS_LeaveLocalRootScope(cx);
		JS_EndRequest(cx);
		ScriptEngine::ReleaseContext(cx);

		for(ArgList::iterator it = evt->args->begin(); it != evt->args->end(); it++)
		{
			if(it->second == JSVal)
				JS_RemoveRoot(&(it->first));
		}

		delete[] argv;

		delete evt->args;
		delete evt;
	}
	else
		DebugBreak(); // TEMP
}
