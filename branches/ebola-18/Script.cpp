#include <io.h>
#include <algorithm>

#include "D2BS.h"
#include "JSGlobalFuncs.h"
#include "JSGlobalClasses.h"
#include "ScriptEngine.h"
#include "Script.h"
#include "Core.h"
#include "Constants.h"
#include "D2Ptrs.h"
#include "Helpers.h"

using namespace std;

Script::Script(const char* file, ScriptType scripttype) :
		context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), execCount(0),
		scriptState(Unexecuted), threadHandle(INVALID_HANDLE_VALUE), threadId(0)
{
	InitializeCriticalSection(&lock);
	EnterCriticalSection(&lock);

	//if(ScriptEngine::GetCount(file)))

	scriptType = scripttype;
	if(scripttype == Command)
	{
		fileName = string("Command Line");
	}
	else
	{
		if(!!_access(file, 0))
			throw std::exception("File not found");

		char* tmpName = _strdup(file);
		if(!tmpName)
			throw std::exception("Could not dup filename");

		_strlwr_s(tmpName, strlen(file)+1);
		fileName = string(tmpName);
		replace(fileName.begin(), fileName.end(), '/', '\\');
		free(tmpName);
	}
	try
	{
		context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!context)
			throw std::exception("Couldn't create the context");

#ifdef DEBUG
		//JS_SetGCZeal(context, 1);
#endif

		JS_SetContextPrivate(context, this);
		JS_SetContextThread(context);
		JS_BeginRequest(context);

		JS_SetErrorReporter(context, reportError);
		JS_SetOperationCallback(context, operationCallback);
		JS_SetOptions(context, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML);
		JS_SetVersion(context, JSVERSION_1_8);

		JSObject* globalObject = JS_NewObject(context, &global_obj, NULL, NULL);
		if(!globalObject)
			throw std::exception("Couldn't create the global object");

		if(JS_InitStandardClasses(context, globalObject) == JS_FALSE)
			throw std::exception("Couldn't init global object classes");
		if(JS_DefineFunctions(context, globalObject, global_funcs) == JS_FALSE)
			throw std::exception("Couldn't define global object functions");

		myUnit* lpUnit = new myUnit;
		memset(lpUnit, NULL, sizeof(myUnit));

		UnitAny* player = D2CLIENT_GetPlayerUnit();
		lpUnit->dwMode = (DWORD)-1;
		lpUnit->dwClassId = (DWORD)-1;
		lpUnit->dwType = UNIT_PLAYER;
		lpUnit->dwUnitId = player ? player->dwUnitId : NULL;
		lpUnit->_dwPrivateType = PRIVATE_UNIT;

		int i = 0;
		for(JSClassSpec entry = global_classes[0]; entry.js_class != NULL; i++, entry = global_classes[i])
			ScriptEngine::InitClass(context, globalObject, entry.js_class, entry.class_ctor,
			entry.funcs, entry.props, entry.static_funcs, entry.static_props);

		JSObject* meObject = BuildObject(context, &unit_class, unit_methods, me_props, lpUnit);
		if(!meObject)
			throw std::exception("Couldn't build me object");

		if(JS_DefineProperty(context, globalObject, "me", OBJECT_TO_JSVAL(meObject), NULL, NULL, JSPROP_CONSTANT) == JS_FALSE)
			throw std::exception("Couldn't define me object properties");

#define DEFCONST(vp) ScriptEngine::DefineConstant(context, globalObject, #vp, vp)
		DEFCONST(FILE_READ);
		DEFCONST(FILE_WRITE);
		DEFCONST(FILE_APPEND);
#undef DEFCONST

		if(scriptType == Command)
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
		JS_ClearContextThread(context);

		LeaveCriticalSection(&lock);
	} catch(std::exception&) {
		if(scriptObject)
			JS_RemoveRoot(&scriptObject);
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
	if(GetScriptState() == Running)
		Stop();

	if(!JS_GetContextThread(context))
		JS_SetContextThread(context);
	JS_BeginRequest(context);

	// these calls can, and probably should, be moved to the context callback on cleanup
	// use the RT version of RemoveRoot to prevent crashes
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &globalObject);
	JS_RemoveRootRT(ScriptEngine::GetRuntime(), &scriptObject);

	JS_EndRequest(context);
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

const char* Script::GetFilename(void)
{
	const char* file = _strdup(fileName.c_str());

#ifdef DEBUG
	if(!file)
		DebugBreak();
#endif

	return file;
}

int Script::GetExecutionCount(void)
{
	return execCount;
}

DWORD Script::GetThreadId(void)
{
	return (threadHandle == NULL ? -1 : threadId);
}

void Script::SetScriptState(ScriptExecState state)
{
	scriptState = state;
	JS_TriggerOperationCallback(context);
}

void Script::Run(void)
{
	if(GetScriptState() == Running)
		DebugBreak();

	SetScriptState(Running);

	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	threadId = GetCurrentThreadId();

	if(threadHandle == INVALID_HANDLE_VALUE)
		DebugBreak();

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	JS_AddRoot(&main);

	if(!JS_GetContextThread(GetContext()))
		JS_SetContextThread(GetContext());
	JS_BeginRequest(GetContext());

	globalObject = JS_GetGlobalObject(GetContext());
	if(JS_ExecuteScript(GetContext(), globalObject, script, &dummy) == JS_FALSE)
	{
		JS_RemoveRoot(&main);
		JS_EndRequest(GetContext());
		return;
	}
	if(JS_GetProperty(GetContext(), globalObject, "main", &main) == JS_FALSE)
	{
		JS_RemoveRoot(&main);
		JS_EndRequest(GetContext());
		return;
	}
	if(JSVAL_IS_FUNCTION(GetContext(), main))
		JS_CallFunctionValue(GetContext(), globalObject, main, 0, NULL, &dummy);

	if(!JS_GetContextThread(GetContext()))
		JS_SetContextThread(GetContext());

	JS_RemoveRoot(&main);
	JS_EndRequest(GetContext());
	JS_ClearContextThread(GetContext());

	execCount++;
}

void Script::Pause(void)
{
	//EnterCriticalSection(&lock);
	if(GetScriptState() == Running)
		SetScriptState(Paused);
	//LeaveCriticalSection(&lock);
}

void Script::Resume(void)
{
	//EnterCriticalSection(&lock);
	if(GetScriptState() == Paused)
		SetScriptState(Running);
	//LeaveCriticalSection(&lock);
}

void Script::Stop(void)
{
#if 0
	int maxCount = (force ? (reallyForce ? 100 : 300) : 500);
	for(int i = 0; IsRunning(); i++)
	{
		// if we pass the time frame, just ignore the wait because the thread will end forcefully anyway
		if(i >= maxCount)
			break;
		Sleep(10);
	}
#endif

	EnterCriticalSection(&lock);

	if(GetScriptState() == Running)
	{
		SetScriptState(Stopped);

		ClearAllEvents();
		Genhook::Clean(this);

		if(threadHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(threadHandle);
			threadHandle = INVALID_HANDLE_VALUE;
		}
		else
			DebugBreak();

		threadId = 0;
	}

	LeaveCriticalSection(&lock);
}

bool Script::IsIncluded(const char* file)
{
	uint count = 0;
	char* fname = _strdup(file);
	if(!fname)
		return false;

	_strlwr_s(fname, strlen(fname)+1);
	StringReplace(fname, '/', '\\');
	count = includes.count(string(fname));
	free(fname);
	fname = NULL;

	return !!count;
}

bool Script::Include(const char* file)
{
	// since includes will happen on the same thread, locking here is acceptable
	EnterCriticalSection(&lock);
	char* fname = _strdup((char*)file);
	if(!fname)
		return false;
	_strlwr_s(fname, strlen(fname)+1);
	StringReplace(fname, '/', '\\');
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(!!includes.count(string(fname)) || !!inProgress.count(string(fname)) || (fileName == string(fname)))
	{
		LeaveCriticalSection(&lock);
		free(fname);
		fname = NULL;
		return true;
	}
	bool rval = false;
	JS_BeginRequest(GetContext());

	JSScript* script = JS_CompileFile(GetContext(), GetGlobalObject(), fname);
	if(script)
	{
		JSObject* scriptObj = JS_NewScriptObject(GetContext(), script);
		JS_AddRoot(&scriptObj);
		jsval dummy;
		inProgress[fname] = true;
		rval = !!JS_ExecuteScript(GetContext(), GetGlobalObject(), script, &dummy);
		if(rval)
			includes[fname] = true;
		inProgress.erase(fname);
		JS_RemoveRoot(&scriptObj);
	}

	JS_EndRequest(GetContext());
	LeaveCriticalSection(&lock);
	free(fname);
	fname = NULL;
	return rval;
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
	if(strlen(evtName) < 1)
		return;
	if(!evtFunc)
		return;

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
	functions.clear();
	LeaveCriticalSection(&lock);
}

void Script::ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv)
{
	if(GetScriptState() != Running || !functions.count(evtName))
	{
		// no event will happen, clean up the roots
		for(uintN i = 0; i < argc; i++)
			delete argv[i];
		delete[] argv;
		return;
	}

	for(uintN i = 0; i < argc; i++)
		argv[i]->Take();

	Event* evt = new Event;
	evt->owner = this;
	evt->dwProduce = GetTickCount();
	evt->functions = functions[evtName];
	evt->argc = argc;
	evt->argv = argv;

	CreateThread(0, 0, FuncThread, evt, 0, 0);
}

DWORD WINAPI ScriptThread(void* data)
{
	Script* script = (Script*)data;
	if(script)
	{
		script->Run();
		script->Stop();
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

#ifdef DEBUG
	if(!evt->owner)
		DebugBreak();
#endif

	evt->dwConsume = GetTickCount();
	static DWORD EvtLFStart = 0;
	DWORD EvtFStart = evt->dwConsume - evt->dwProduce;
	if(EvtFStart > EvtLFStart)
	{
		EvtLFStart = EvtFStart;
		Print("Longest event took %lu ms to get to functhread.", EvtLFStart);
	}

	// TODO see what happens with console command if it spawns an event .. if it can
	if(evt->owner->GetScriptState() == Running && !(evt->owner->GetScriptType() == InGame && !GameReady()))
	{
		jsval dummy = JSVAL_VOID;

		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = evt->argv[i]->value();
			if(JS_AddRoot(&args[i]) == JS_FALSE)
			{
				if(evt->argv)
				{
					delete[] evt->argv;
					evt->argv = NULL;
				}
				delete evt;
				evt = NULL;
				return 0;
			}
		}

		JSContext* context = JS_NewContext(ScriptEngine::GetRuntime(), 0x2000);
		if(!context)
		{
			if(evt->argv)
			{
				delete[] evt->argv;
				evt->argv = NULL;
			}
			delete evt;
			evt = NULL;
			return 0;
		}

		JSObject* object = evt->owner->GetGlobalObject();
		JS_SetContextPrivate(context, evt->owner);
		JS_BeginRequest(context);

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
		{
			if(!JS_CallFunctionValue(context, object, (*it)->value(), evt->argc, args, &dummy))
				break;
		}

		static DWORD EvtLFEnd = 0;
		DWORD EvtFEnd = GetTickCount() - evt->dwProduce;
		if(EvtFEnd > EvtLFEnd)
		{
			EvtLFEnd = EvtFEnd;
			Print("Longest event took %lu ms to get to complete.", EvtLFEnd);
		}

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(&args[i]);
		delete[] args;
		args = NULL;

		JS_EndRequest(context);
		JS_DestroyContext(context);
	}

	// assume we have to clean up both the event and the args, and release autorooted vars
	for(uintN i = 0; i < evt->argc; i++)
	{
		evt->argv[i]->Release();
		if(evt->argv[i])
		{
			delete evt->argv[i];
			evt->argv[i] = NULL;
		}
	}
	if(evt->argv)
	{
		delete[] evt->argv;
		evt->argv = NULL;
	}
	delete evt;
	evt = NULL;

	return 0;
}
