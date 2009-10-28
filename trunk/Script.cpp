#include <io.h>
#include <algorithm>

#include "Script.h"
#include "Core.h"
#include "Constants.h"
#include "D2Ptrs.h"
#include "CDebug.h"
#include "Helpers.h"
#include "ScriptEngine.h"
#include "D2BS.h"

#include "debugnew/debug_new.h"

using namespace std;

AutoRoot::AutoRoot(jsval nvar) : var(nvar), count(0) { Take(); }
AutoRoot::~AutoRoot()
{
	if(!(count--))
	{
		fprintf(stderr, "AutoRoot failed: Count is still %i, but the root is being destroyed", count);
		DebugBreak();
		exit(3);
	}
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &var);
}

void AutoRoot::Take() 
{ 
	count++;
	JS_AddNamedRootRT(ScriptEngine::GetRuntime(), &var, "AutoRoot");
}

void AutoRoot::Release()
{
	count--;
	if(count < 0)
	{
		fprintf(stderr, "Improper AutoRoot usage: Count is less than 0");
		DebugBreak();
		exit(3);
	}
}
jsval AutoRoot::operator* () { return var; }
jsval AutoRoot::value() { return var; }
bool AutoRoot::operator==(AutoRoot& other) { return other.value() == var; }

Script::Script(const char* file, ScriptState state) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), execCount(0),
			isAborted(false), isPaused(false), isReallyPaused(false), singleStep(false),
			scriptState(state), threadHandle(INVALID_HANDLE_VALUE), threadId(0)
{
	if(scriptState != Command && _access(file, 0) != 0)
		throw std::exception("File not found");

	InitializeCriticalSection(&lock);
	EnterCriticalSection(&lock);

	fileName = string(_strlwr(_strdup(file)));
	replace(fileName.begin(), fileName.end(), '/', '\\');
	try {
		context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!context)
			throw std::exception("Couldn't create the context");

		JS_SetContextThread(context);
		JS_SetContextPrivate(context, this);
		JS_BeginRequest(context);

		globalObject = JS_GetGlobalObject(context);

		if(state == Command)
			script = JS_CompileScript(context, globalObject, file, strlen(file), "Command Line", 1);
		else
			script = JS_CompileFile(context, globalObject, fileName.c_str());
		if(!script)
			throw std::exception("Couldn't compile the script");

		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Couldn't create the script object");

		if(JS_AddNamedRoot(context, &scriptObject, "script object") == JS_FALSE)
			throw std::exception("Couldn't add named root for scriptObject");

		JS_EndRequest(context);

		LeaveCriticalSection(&lock);
	} catch(std::exception&) {
		if(scriptObject)
			JS_RemoveRoot(context, &scriptObject);
		if(script && !scriptObject)
			JS_DestroyScript(context, script);
		if(context)
		{
			JS_EndRequest(context);
			JS_DestroyContext(context);
		}
		LeaveCriticalSection(&lock);
		throw;
	}
}

Script::~Script(void)
{
	EnterCriticalSection(&lock);
	Stop(true, true);

	JS_SetContextThread(context);
	JS_BeginRequest(context);

	// these calls can, and probably should, be moved to the context callback on cleanup
	// use the RT version of RemoveRoot to prevent crashes
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &globalObject);
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &scriptObject);

	JS_DestroyContext(context);

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;

	includes.clear();
	if(threadHandle)
		CloseHandle(threadHandle);
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
	// only let the script run if it's not already running
	if(IsRunning())
		return;

	isAborted = false;
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	threadId = GetCurrentThreadId();

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	JS_AddRoot(GetContext(), &main);

	JS_SetContextThread(GetContext());
	JS_BeginRequest(GetContext());

	if(JS_ExecuteScript(GetContext(), globalObject, script, &dummy) == JS_FALSE)
	{
		JS_RemoveRoot(GetContext(), &main);
		JS_EndRequest(GetContext());
		return;
	}
	if(JS_GetProperty(GetContext(), globalObject, "main", &main) == JS_FALSE)
	{
		JS_RemoveRoot(GetContext(), &main);
		JS_EndRequest(GetContext());
		return;
	}
	if(JSVAL_IS_FUNCTION(GetContext(), main))
		JS_CallFunctionValue(GetContext(), globalObject, main, 0, NULL, &dummy);

	JS_SetContextThread(GetContext());
	JS_RemoveRoot(GetContext(), &main);
	JS_EndRequest(GetContext());	

	execCount++;
	Stop();
}

void Script::Pause(void)
{
	//EnterCriticalSection(&lock);
	if(!IsAborted() && !IsPaused())
		isPaused = true;
	//LeaveCriticalSection(&lock);
}

void Script::Resume(void)
{
	//EnterCriticalSection(&lock);
	if(!IsAborted() && IsPaused())
		isPaused = false;
	//LeaveCriticalSection(&lock);
}

bool Script::IsPaused(void)
{
	return IsBadReadPtr(this, sizeof(this)) ? false : isPaused;
}

void Script::Stop(bool force, bool reallyForce)
{	
	// Clear the events/hooks before aborting the script
	// otherwise we can't clean up all the events in the context
	EnterCriticalSection(&lock);
	ClearAllEvents();
	Genhook::Clean(this);

	isAborted = true;
	isPaused = false;
	isReallyPaused = false;

	int maxCount = (force ? (reallyForce ? 100 : 300) : 500);
	for(int i = 0; IsRunning(); i++)
	{
		// if we pass the time frame, just ignore the wait because the thread will end forcefully anyway
		if(i >= maxCount)
			break;
		Sleep(10);
	}

	if(threadHandle)
		CloseHandle(threadHandle);
	threadHandle = NULL;
	LeaveCriticalSection(&lock);
}

void Script::EnableSingleStep(void) { singleStep = true; }
void Script::DisableSingleStep(void) { singleStep = false; }
bool Script::IsSingleStep(void) { return singleStep; }

bool Script::IsIncluded(const char* file)
{
	char* fname = _strlwr((char*)file);
	StringReplace(fname, '/', '\\');
	return !!includes.count(string(fname));
}

bool Script::Include(const char* file)
{
	// since includes will happen on the same thread, locking here is acceptable
	EnterCriticalSection(&lock);
	char* fname = _strlwr((char*)file);
	StringReplace(fname, '/', '\\');
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(IsIncluded(fname) || !!inProgress.count(string(fname)) || (fileName == string(fname)))
	{
		LeaveCriticalSection(&lock);
		return true;
	}
	bool rval = false;
	JS_BeginRequest(GetContext());

	JSScript* script = JS_CompileFile(GetContext(), globalObject, fname);
	if(script)
	{
		jsval dummy;
		inProgress[fname] = true;
		rval = !!JS_ExecuteScript(GetContext(), globalObject, script, &dummy);
		JS_DestroyScript(GetContext(), script);
		if(rval)
			includes[fname] = true;
		inProgress.erase(fname);
	}
	else
	{
		JS_SetContextThread(GetContext());
		JS_EndRequest(GetContext());
		LeaveCriticalSection(&lock);
		return false;
	}

	// HACK: assume we have to reclaim ownership
	JS_SetContextThread(GetContext());
	JS_EndRequest(GetContext());
	LeaveCriticalSection(&lock);
	return rval;
}

bool Script::IsRunning(void)
{
	return !IsBadReadPtr(this, sizeof(this)) && !IsBadReadPtr(context, sizeof(context)) &&
					context && !(!JS_IsRunning(context) || IsPaused());
}

bool Script::IsAborted()
{
	return IsBadReadPtr(this, sizeof(this)) ? true : isAborted;
}

bool Script::IsListenerRegistered(const char* evtName)
{
	// nothing can be registered under an empty name
	if(strlen(evtName) < 1)
		return false;

	if(functions.count(evtName) > 0)
		return true;

	return false;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	EnterCriticalSection(&lock);
	if(JSVAL_IS_FUNCTION(context, evtFunc) && strlen(evtName) > 0)
	{
		AutoRoot* root = new AutoRoot(evtFunc);
		root->Take();
		functions[evtName].push_back(root);
	}
	LeaveCriticalSection(&lock);
}

bool Script::IsRegisteredEvent(const char* evtName, jsval evtFunc)
{
	// nothing can be registered under an empty name
	if(strlen(evtName) < 1)
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
	functions[evtName].remove(func);
	func->Release();
	delete func;
	LeaveCriticalSection(&lock);
}

void Script::ClearEvent(const char* evtName)
{
	EnterCriticalSection(&lock);
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		AutoRoot* func = *it;
		func->Release();
		delete func;
	}
	functions[evtName].clear();
	LeaveCriticalSection(&lock);
}

void Script::ClearAllEvents(void)
{
	EnterCriticalSection(&lock);
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
	LeaveCriticalSection(&lock);
}

void Script::ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv)
{
	if(!(!IsAborted() && !IsPaused() && functions.count(evtName)))
	{
		// no event will happen, clean up the roots
		for(uintN i = 0; i < argc; i++)
			delete argv[i];
		delete[] argv;
		return;
	}

	char msg[50];
	sprintf_s(msg, sizeof(msg), "Script::ExecEventAsync(%s)", evtName);
	CDebug cDbg(msg);

	for(uintN i = 0; i < argc; i++)
		argv[i]->Take();

	Event* evt = new Event;
	evt->owner = this;
	evt->functions = functions[evtName];
	evt->argc = argc;
	evt->argv = argv;
	evt->context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
	if(!evt->context)
	{
		delete evt;
		return;
	}
	evt->object = globalObject;
	JS_SetContextPrivate(evt->context, this);

	CreateThread(0, 0, FuncThread, evt, 0, 0);
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

DWORD WINAPI FuncThread(void* data)
{
	Event* evt = (Event*)data;
	if(!evt)
		return 0;

	JS_SetContextThread(evt->context);
	JS_BeginRequest(evt->context);

	if(evt->owner->IsRunning() && !(evt->owner->GetState() == InGame && !GameReady()))
	{
		jsval dummy = JSVAL_VOID;

		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = evt->argv[i]->value();
			if(JS_AddRoot(evt->context, &args[i]) == JS_FALSE)
			{
				if(evt->argv)
					delete[] evt->argv;
				delete evt;
				return NULL;
			}
		}

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			JS_CallFunctionValue(evt->context, evt->object, (*it)->value(), evt->argc, args, &dummy);
		}

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(evt->context, &args[i]);
		delete[] args;

		// check if the caller stole the context thread
		if ((DWORD)JS_GetContextThread(evt->context) != evt->owner->GetThreadId())
		{
			JS_ClearContextThread(evt->context);
			JS_SetContextThread(evt->context);	
		}
	}

	JS_DestroyContextMaybeGC(evt->context);
	// assume we have to clean up both the event and the args, and release autorooted vars
	for(uintN i = 0; i < evt->argc; i++)
	{
		evt->argv[i]->Release();
		if(evt->argv[i])
			delete evt->argv[i];
	}
	if(evt->argv)
		delete[] evt->argv;
	delete evt;
	
	return 0;
}
