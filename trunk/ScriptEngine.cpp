#include <algorithm>
#include <vector>
#include <stdarg.h>
#include <process.h>

#include "ScriptEngine.h"
#include "Core.h"
#include "JSGlobalFuncs.h"
#include "JSGlobalClasses.h"
#include "JSUnit.h"
#include "Constants.h"
#include "D2BS.h"
#include "Console.h"

using namespace std;

JSRuntime* ScriptEngine::runtime = NULL;
ScriptMap ScriptEngine::scripts = ScriptMap();
ContextList ScriptEngine::active = ContextList();
ContextList ScriptEngine::inactive = ContextList();
EngineState ScriptEngine::state = Stopped;
CRITICAL_SECTION ScriptEngine::lock = {0};
SLIST_HEADER ScriptEngine::eventList = {0};
HANDLE ScriptEngine::eventHandle = INVALID_HANDLE_VALUE;
JSContext* ScriptEngine::context = {0};

// internal ForEachScript helpers
bool __fastcall DisposeScript(Script* script, void*, uint);
bool __fastcall StopScript(Script* script, void* argv, uint argc);
bool __fastcall GCPauseScript(Script* script, void* argv, uint argc);

Script* ScriptEngine::CompileFile(const std::string &file, ScriptType scriptType, bool recompile)
{
	if(GetState() != Running)
		return NULL;

	std::string File = file;
	std::transform(File.begin(), File.end(), File.begin(), tolower);

	try
	{
		EnterCriticalSection(&lock);
		if(!Vars.bDisableCache)
		{
			if(recompile && scripts.count(File) > 0)
			{
				scripts[File]->Stop(true);
				DisposeScript(scripts[File]);
			}
			else if(scripts.count(File) > 0)
			{
				Script* ret = scripts[File];
				ret->Stop(true);
				LeaveCriticalSection(&lock);
				return ret;
			}
		}
		Script* script = new Script(File, scriptType);
		scripts[File] = script;
		LeaveCriticalSection(&lock);

		return script;
	}
	catch(std::exception &e)
	{
		LeaveCriticalSection(&lock);
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

Script* ScriptEngine::CompileCommand(const char* command)
{
	if(GetState() != Running)
		return NULL;

	try
	{
		EnterCriticalSection(&lock);
		if(!Vars.bDisableCache)
			if(scripts.count("Command Line") > 0)
				DisposeScript(scripts["Command Line"]);

		Script* script = new Script(command, Command);
		scripts["Command Line"] = script;
		LeaveCriticalSection(&lock);
		return script;
	}
	catch(std::exception e)
	{
		LeaveCriticalSection(&lock);
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

void ScriptEngine::DisposeScript(Script* script)
{
	EnterCriticalSection(&lock);

	if(scripts.count(script->GetFilename()))
		scripts.erase(script->GetFilename());

	LeaveCriticalSection(&lock);

	delete script;
}

JSContext* ScriptEngine::AcquireContext(void)
{
	// if we don't have any more contexts, make one
	if(inactive.empty())
	{
		JSContext* cx = JS_NewContext(runtime, 8192);
		JS_ClearContextThread(cx);
		inactive.insert(cx);
	}

	ASSERT(!inactive.empty());

	JSContext* result = *inactive.begin();
	inactive.erase(result);
	active.insert(result);
	JS_SetContextThread(result);

	return result;
}

void ScriptEngine::ReleaseContext(JSContext* context)
{
	ASSERT(!active.empty() && active.find(context) != active.end());

	active.erase(context);
	inactive.insert(context);
	JS_ClearContextThread(context);

	ASSERT(!inactive.empty());
}

unsigned int ScriptEngine::GetCount(bool active, bool unexecuted)
{
	if(GetState() != Running)
		return 0;

	EnterCriticalSection(&lock);
	int count = scripts.size();
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		//if(!active && it->second->IsRunning() && it->second->GetExecState() != ScriptStateAborted)
		if(!active && it->second->GetExecState() == ScriptStateRunning)
			count--;
		//if(!unexecuted && it->second->GetExecutionCount() == 0 && !it->second->IsRunning())
		if(!unexecuted && it->second->GetExecutionCount() == 0 && it->second->GetExecState() != ScriptStateRunning)
			count--;
	}
	assert(count >= 0);
	LeaveCriticalSection(&lock);
	return count;
}

BOOL ScriptEngine::Startup(void)
{
	static bool initialized = false;
	if(GetState() == Stopped)
	{
		if(!initialized)
			InitializeCriticalSection(&lock);
		EnterCriticalSection(&lock);

		state = Starting;

		InitializeSListHead(&eventList);

		eventHandle = (HANDLE)_beginthread(EventThread, 0, NULL);

		// create the runtime with the requested memory limit
		runtime = JS_NewRuntime(Vars.dwMemUsage);
		if(!runtime)
		{
			LeaveCriticalSection(&lock);
			return FALSE;
		}
		// grab a private context to use before we assign the context callback
		context = JS_NewContext(runtime, 8192);
		//JS_SetGCZeal(context, 1);
		JS_ClearContextThread(context);

		JS_SetContextCallback(runtime, contextCallback);
		JS_SetGCCallbackRT(runtime, gcCallback);

		state = Running;
		LeaveCriticalSection(&lock);
	}
	return TRUE;
}

void ScriptEngine::Shutdown(void)
{
	if(GetState() == Running)
	{
		// bring the engine down properly
		EnterCriticalSection(&lock);
		state = Stopping;
		StopAll(true);
		WaitForSingleObject(eventHandle, 500);

		// clear all scripts now that they're stopped
		ForEachScript(::DisposeScript, NULL, 0);

		if(!scripts.empty())
			scripts.clear();

		if(runtime)
		{
			int count = active.size();
			for(int i = 0; i < count; i++)
			{
				JSContext* cx = *active.end();
				JS_DestroyContextNoGC(cx);
			}
			count = inactive.size();
			for(int i = 0; i < count; i++)
			{
				JSContext* cx = *inactive.end();
				JS_DestroyContextNoGC(cx);
			}
			active.clear();
			inactive.clear();

			// free our private context just before we shut down the runtime
			JS_DestroyContext(context);
			JS_DestroyRuntime(runtime);
			JS_ShutDown();
			runtime = NULL;
		}

		LeaveCriticalSection(&lock);
		DeleteCriticalSection(&lock);
		state = Stopped;
	}
}

void ScriptEngine::StopAll(bool forceStop)
{
	if(GetState() != Running)
		return;

	EnterCriticalSection(&lock);

	ForEachScript(StopScript, &forceStop, 1);

	LeaveCriticalSection(&lock);
}

void ScriptEngine::FlushCache(void)
{
	if(GetState() != Running)
		return;

	static bool isFlushing = false;

	if(isFlushing || Vars.bDisableCache)
		return;

	EnterCriticalSection(&lock);
	// TODO: examine if this lock is necessary any more
	EnterCriticalSection(&Vars.cFlushCacheSection);

	isFlushing = true;

	ForEachScript(::DisposeScript, NULL, 0);

	isFlushing = false;

	LeaveCriticalSection(&Vars.cFlushCacheSection);
	LeaveCriticalSection(&lock);
}

void ScriptEngine::ForEachScript(ScriptCallback callback, void* argv, uint argc)
{
	if(callback == NULL || scripts.size() < 1)
		return;

	EnterCriticalSection(&lock);

	// damn std::list not supporting operator[]...
	std::vector<Script*> list;
	for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		list.push_back(it->second);
	int count = list.size();
	// damn std::iterator not supporting manipulating the list...
	for(int i = 0; i < count; i++)
	{
		if(!callback(list[i], argv, argc))
			break;
	}

	LeaveCriticalSection(&lock);
}

void ScriptEngine::PushEvent(EventHelper* helper)
{
	InterlockedPushEntrySList(&eventList, (PSINGLE_LIST_ENTRY)helper);
}

void ScriptEngine::ExecEventAsync(char* evtName, char* format, ...)
{
	if(GetState() != Running)
		return;

	va_list args;
	va_start(args, format);
	uintN len = strlen(format);
	char fmt[10];
	strcpy_s(fmt, 10, format);
	ArgList* argv = new ArgList(len);

	for(uintN i = 0; i < len; i++)
	{
		switch(format[i])
		{
			case 'i': case 'j':
				argv->at(i) = (make_pair((QWORD)va_arg(args, int32), SignedInt));
				break;
			case 'd': case 'I':
				argv->at(i) = (make_pair((QWORD)va_arg(args, jsdouble), Double));
				break;
			case 'b':
				argv->at(i) = (make_pair((QWORD)va_arg(args, JSBool), Boolean));
				break;
			case 'c':
				argv->at(i) = (make_pair((QWORD)va_arg(args, uint16), UnsignedShort));
				break;
			case 'u':
				argv->at(i) = (make_pair((QWORD)va_arg(args, uint32), UnsignedInt));
				break;
			case 'v': {
					EventArg p = make_pair((QWORD)va_arg(args, jsval), JSVal);
					argv->at(i) = p;
					JS_AddRoot(&(p.first));
					break;
				}
			case 's': {
				JS_SetContextThread(context);
				JS_BeginRequest(context);

				char* str = va_arg(args, char*);
				// HACK we probably don't want to be interning every string, since that
				// will just swell our memory usage with no means to clear it out...
				JSString* encString = JS_InternString(context, str);
				fmt[i] = 'S';
				EventArg p = make_pair((QWORD)encString, String);
				argv->at(i) = p;

				JS_EndRequest(context);
				JS_ClearContextThread(context);
				break;
			}
			default:
			{
				// give a named assert instead of just a false
// we know the local variable is initialized but not referenced, and it actually is referenced
#pragma warning ( disable : 4189 )
				bool api_usage_error = false;
				ASSERT(api_usage_error);
#pragma warning ( default : 4189 )
				break;
			}
		}
	}
	va_end(args);

	EventHelper* helper = new EventHelper;
	strcpy_s(helper->evtName, 15, evtName);
	strcpy_s(helper->format, 10, fmt);
	helper->executed = false;
	helper->args = argv;

	PushEvent(helper);
}

void ScriptEngine::InitClass(JSContext* context, JSObject* globalObject, JSClass* classp,
							 JSFunctionSpec* methods, JSPropertySpec* props,
							 JSFunctionSpec* s_methods, JSPropertySpec* s_props)
{
	if(!JS_InitClass(context, globalObject, NULL, classp, classp->construct, 0, props, methods, s_props, s_methods))
		throw std::exception("Couldn't initialize the class");
}

void ScriptEngine::DefineConstant(JSContext* context, JSObject* globalObject, const char* name, int value)
{
	if(!JS_DefineProperty(context, globalObject, name, INT_TO_JSVAL(value), NULL, NULL, JSPROP_PERMANENT_VAR))
		throw std::exception("Couldn't initialize the constant");
}

// internal ForEachScript helper functions
bool __fastcall DisposeScript(Script* script, void*, uint)
{
	ScriptEngine::DisposeScript(script);
	return true;
}

bool __fastcall StopScript(Script* script, void* argv, uint argc)
{
	script->Stop(ScriptEngine::GetState() == Stopping);
	return true;
}

bool __fastcall StopIngameScript(Script* script, void*, uint)
{
	if(script->GetScriptType() == InGame)
		script->Stop(true);
	return true;
}

bool __fastcall ExecEventOnScript(Script* script, void* argv, uint argc)
{
	EventHelper* helper = (EventHelper*)argv;
	ASSERT(helper);

	if(script->GetExecState() == ScriptStateRunning && script->IsListenerRegistered(helper->evtName))
		script->ExecEventAsync(helper->evtName, helper->format, helper->args);
	//else
	//	DebugBreak();

	return true;
}

JSBool branchCallback(JSContext* cx)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	if(script->GetExecState() == ScriptStateAborted)
		return JS_FALSE;

	if(script->GetExecState() == ScriptStatePaused)
	{
		jsrefcount depth = JS_SuspendRequest(cx);

		JS_MaybeGC(cx);
		while(script->GetExecState() == ScriptStatePaused)
			Sleep(10);

		JS_ResumeRequest(cx, depth);
	}

	script->UpdatePlayerGid();

	return !!!(script->GetScriptType() != OutOfGame && ClientState() != ClientStateInGame);
}

JSBool contextCallback(JSContext* cx, uintN contextOp)
{
	if(contextOp == JSCONTEXT_NEW)
	{
		JS_BeginRequest(cx);

		JS_SetErrorReporter(cx, reportError);
		JS_SetOperationCallback(cx, branchCallback);
		JS_SetOptions(cx, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML);
		JS_SetVersion(cx, JSVERSION_LATEST);

		JSObject* globalObject = JS_NewObject(cx, &global_obj, NULL, NULL);
		if(!globalObject)
			return JS_FALSE;

		if(JS_InitStandardClasses(cx, globalObject) == JS_FALSE)
			return JS_FALSE;
		if(JS_DefineFunctions(cx, globalObject, global_funcs) == JS_FALSE)
			return JS_FALSE;

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
			ScriptEngine::InitClass(cx, globalObject, entry.js_class, entry.funcs, entry.props,
										entry.static_funcs, entry.static_props);

		JSObject* meObject = BuildObject(cx, &unit_class_ex.base, unit_methods, me_props, lpUnit);
		if(!meObject)
			return JS_FALSE;

		if(JS_DefineProperty(cx, globalObject, "me", OBJECT_TO_JSVAL(meObject), NULL, NULL, JSPROP_PERMANENT_VAR) == JS_FALSE)
			return JS_FALSE;

#define DEFCONST(vp) ScriptEngine::DefineConstant(cx, globalObject, #vp, vp)
		DEFCONST(FILE_READ);
		DEFCONST(FILE_WRITE);
		DEFCONST(FILE_APPEND);
#undef DEFCONST

		JS_EndRequest(cx);
	}
	return JS_TRUE;
}

JSBool gcCallback(JSContext *cx, JSGCStatus status)
{
	if(status == JSGC_BEGIN)
	{
//		EnterCriticalSection(&ScriptEngine::lock);
#ifdef DEBUG
		Log("*** ENTERING GC ***");
#endif
	}
	else if(status == JSGC_END)
	{
#ifdef DEBUG
		Log("*** LEAVING GC ***");
#endif
#endif
//		LeaveCriticalSection(&ScriptEngine::lock);
	}
	return JS_TRUE;
}

void __cdecl EventThread(void* arg)
{
	while(ScriptEngine::GetState() != Stopped)
	{
		while(QueryDepthSList(&ScriptEngine::eventList) > 0)
		{
			EventHelper* helper = (EventHelper*)InterlockedPopEntrySList(&ScriptEngine::eventList);
			if(helper)
			{
				// execute it on every script
				ASSERT(helper);
				ScriptEngine::ForEachScript(ExecEventOnScript, helper, 1);
				delete helper;
			}
		}
		Sleep(1);
	}
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	bool warn = JSREPORT_IS_WARNING(report->flags);
	bool isStrict = JSREPORT_IS_STRICT(report->flags);
	const char* type = (warn ? "Warning" : "Error");
	const char* strict = (isStrict ? "Strict " : "");
	char* filename = report->filename ? _strdup(report->filename) : _strdup("<unknown>");
	char* displayName = filename;
	if(_stricmp("Command Line", filename) != 0 && _stricmp("<unknown>", filename) != 0)
		displayName = filename + strlen(Vars.szPath);

	Log("[%s%s] Code(%d) File(%s:%d) %s\nLine: %s", 
			strict, type, report->errorNumber, filename, report->lineno, message, report->linebuf);

	Print("[ÿc%d%s%sÿc0 (%d)] File(%s:%d) %s", 
			(warn ? 9 : 1), strict, type, report->errorNumber, displayName, report->lineno, message);

	if(filename)
		free(filename);

	if(Vars.bQuitOnError && !JSREPORT_IS_WARNING(report->flags) && ClientState() == ClientStateInGame)
		D2CLIENT_ExitGame();
	else
		Console::ShowBuffer();
}
