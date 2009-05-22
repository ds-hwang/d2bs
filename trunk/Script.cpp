#include "Script.h"
#include "JSGlobalFuncs.h"
#include "Core.h"
#include <io.h>
#include "JSUnit.h"
#include "Constants.h"
#include "D2Ptrs.h"
#include "D2BS.h"
#include "CDebug.h"
#include "Helpers.h"

#include "debugnew/debug_new.h"

using namespace std;

AutoRoot::AutoRoot(JSContext* cx, jsval nvar) :	context(cx), var(nvar), count(0) { Take(); }
AutoRoot::~AutoRoot()
{
	if(!(count--))
	{
		fprintf(stderr, "AutoRoot failed: Count is still %i, but the root is being destroyed", count);
		DebugBreak();
		exit(3);
	}
	Script::LockAll();
	JS_RemoveRoot(context, &var);
	Script::UnlockAll();
}
void AutoRoot::Take() { count++; Script::LockAll(); JS_AddRoot(context, &var); Script::UnlockAll(); }
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
jsval AutoRoot::value() { return var; }
bool AutoRoot::operator==(AutoRoot& other) { return other.value() == var; }

JSRuntime* Script::runtime = NULL;
ScriptMap Script::activeScripts = ScriptMap();
CRITICAL_SECTION Script::criticalSection = {0};
bool Script::isAllLocked = false;

Script* Script::CompileFile(const char* file, ScriptState state, bool recompile)
{
	file = _strlwr(_strdup(file));
	try {
		if(recompile && activeScripts.count(file) > 0) {
			activeScripts[file]->Stop(true, true);
			delete activeScripts[file];
		} else if(activeScripts.count(file) > 0) {
			activeScripts[file]->Stop(true, true);
			return activeScripts[file];
		}
		Script* script = new Script(file, state);
		return script;
	} catch(std::exception e) {
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

Script* Script::CompileCommand(const char* command)
{
	try {
		if(activeScripts.count(_strlwr(_strdup(command))) > 0) {
			return activeScripts[_strlwr(_strdup(command))];
		}
		Script* script = new Script(command, Command);
		return script;
	} catch(std::exception e) {
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

Script::Script(const char* file, ScriptState state) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), fileName(NULL),
			execCount(0), isAborted(false), isPaused(false), isReallyPaused(false), singleStep(false), scriptState(state),
			threadHandle(NULL), threadId(0), lockThreadId(-1)
{
	if(scriptState != Command && _access(file, 0) != 0)
		throw std::exception("File not found");

	InitializeCriticalSection(&scriptSection);
	fileName = _strlwr(_strdup(file));
	StringReplace(fileName, '/', '\\');
	try {
		AutoLock lock(this);
		context = BuildContext(GetRuntime());
		if(!context)
			throw std::exception("Couldn't create the context");
		JS_SetContextThread(context);
		JS_BeginRequest(context);
		JS_SetContextPrivate(context, this);
		globalObject = JS_NewObject(context, &global_obj, NULL, NULL);
		if(!globalObject)
			throw std::exception("Couldn't create the global object");

		JS_InitStandardClasses(context, globalObject);
		JS_DefineFunctions(context, globalObject, global_funcs);
		JS_AddRoot(context, &globalObject);// added for testing proper destroy context code

		InitClass(&file_class_ex.base, file_methods, file_props, file_s_methods, NULL);
		InitClass(&filetools_class, NULL, NULL, filetools_s_methods, NULL);
		InitClass(&sqlite_db_ex.base, sqlite_methods, sqlite_props, NULL, NULL);
		InitClass(&sandbox_class, sandbox_methods, NULL, NULL, NULL);
		InitClass(&frame_class, frame_methods, frame_props, NULL, NULL);
		InitClass(&box_class, box_methods, box_props, NULL, NULL);
		InitClass(&line_class, line_methods, line_props, NULL, NULL);
		InitClass(&text_class, text_methods, text_props, NULL, NULL);
		InitClass(&image_class, image_methods, image_props, NULL, NULL);
		JS_DefineObject(context, globalObject, "Unit", &unit_class, NULL, NULL);
		myUnit* lpUnit = new myUnit; // leaked
		memset(lpUnit, NULL, sizeof(myUnit));

		UnitAny* player = D2CLIENT_GetPlayerUnit();
		lpUnit->dwMode = -1;
		lpUnit->dwClassId = -1;
		lpUnit->dwType = UNIT_PLAYER;
		lpUnit->dwUnitId = player ? player->dwUnitId : NULL;
		lpUnit->_dwPrivateType = PRIVATE_UNIT;

		meObject = BuildObject(context, &unit_class, unit_methods, me_props, lpUnit);
		if(!meObject)
			throw std::exception("Couldn't create the meObject");

		JS_AddRoot(context, &meObject);
		JS_SetContextThread(context);
		JS_DefineProperty(context, globalObject, "me", OBJECT_TO_JSVAL(meObject), NULL, NULL, JSPROP_CONSTANT);

#define DEFCONST(vp) DefineConstant(#vp, vp)
#define DEFEVENT(vp) DEFCONST(EVENT_##vp)
		DEFCONST(FILE_READ);
		DEFCONST(FILE_WRITE);
		DEFCONST(FILE_APPEND);

		DEFEVENT(AREACHANGE);
		DEFEVENT(CHATMSG);
		DEFEVENT(COPYDATA);
		DEFEVENT(GAMEMSG);
		DEFEVENT(HOSTILEMSG);
		DEFEVENT(INPUTLINE);
		DEFEVENT(ITEMSTAT);
		DEFEVENT(KEYDOWN);
		DEFEVENT(KEYUP);
		DEFEVENT(MELIFE);
		DEFEVENT(MISSILEMOVE);
		DEFEVENT(MISSILESTATE);
		DEFEVENT(MOUSEDOWN);
		DEFEVENT(MOUSEUP);
		DEFEVENT(NEWITEM);
		DEFEVENT(NEWNPC);
		DEFEVENT(NPCLIFE);
		DEFEVENT(NPCMOVE);
		DEFEVENT(NPCSTAT);
		DEFEVENT(NPCSTATE);
		DEFEVENT(PARTYMSG);
		DEFEVENT(PLAYERMOVE);
		DEFEVENT(PLAYERSTAT);
		DEFEVENT(PLAYERSTATE);
		DEFEVENT(QUEST);
		DEFEVENT(SCRIPTMSG);
		DEFEVENT(UNITMOVE);
		DEFEVENT(WINDOWFOCUS);
		DEFEVENT(CHATCMD);
		DEFEVENT(PLAYERASSIGN);
#undef DEFEVENT
#undef DEFCONST

		if(state == Command)
			script = JS_CompileScript(context, globalObject, file, strlen(file), "Command Line", 1);
		else
			script = JS_CompileFile(context, globalObject, fileName);
		if(!script)
			throw std::exception("Couldn't compile the script");

		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw std::exception("Couldn't create the script object");

		JS_AddNamedRoot(context, &meObject, "me object");
		JS_AddNamedRoot(context, &scriptObject, "script object");
		JS_EndRequest(context);
		JS_ClearContextThread(context);
		RegisterScript(this);
	} catch(std::exception&) {
		DeleteCriticalSection(&scriptSection);
		if(scriptObject)
			JS_RemoveRoot(context, &scriptObject);
		if(script && !scriptObject)
			JS_DestroyScript(context, script);
		if(context)
			JS_DestroyContext(context);
		throw;
	}
}

Script::~Script(void)
{
	Lock();
	Stop(true, true);
	activeScripts.erase(fileName);

	// use the RT version of RemoveRoot to prevent crashes
	JS_RemoveRootRT(runtime, &globalObject);
	JS_RemoveRootRT(runtime, &meObject);
	JS_RemoveRootRT(runtime, &scriptObject);

	JS_SetContextThread(context);
	if(Vars.bDisableCache)
		JS_DestroyContext(context);
	else
		JS_DestroyContextNoGC(context);

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;
	free(fileName);

	includes.clear();
	functions.clear();
	if(threadHandle)
		CloseHandle(threadHandle);
	Unlock();
	DeleteCriticalSection(&scriptSection);
}

void Script::InitClass(JSClass* classp, JSFunctionSpec* methods, JSPropertySpec* props,
					   JSFunctionSpec* s_methods, JSPropertySpec* s_props)
{
	if(!JS_InitClass(context, globalObject, NULL, classp, classp->construct, 0,
		props, methods, s_props, s_methods))
		throw std::exception("Couldn't initialize the class");
}

void Script::DefineConstant(const char* name, int value)
{
	if(!JS_DefineProperty(context, globalObject, name, INT_TO_JSVAL(value), NULL, NULL, JSPROP_CONSTANT))
		throw std::exception("Couldn't initialize the constant");
}

void Script::Startup(void)
{
	if(!runtime)
	{
		// set the memory limit at 200mb
		runtime = JS_NewRuntime(0xC80000);
		JS_SetGCCallbackRT(runtime, gcCallback);
		//JS_SetThrowHook(runtime, exceptionCallback, NULL);
		//JS_SetCallHook(runtime, executeCallback, NULL);
		//JS_SetExecuteHook(runtime, executeCallback, NULL);
		//JS_SetDebuggerHandler(runtime, debuggerCallback, NULL);
		InitializeCriticalSection(&criticalSection);
	}
}

void Script::Shutdown(void)
{
	LockAll();
	StopAll(true);
	FlushCache();

	activeScripts.clear();

	if(runtime)
	{
		JS_DestroyRuntime(runtime);
		JS_ShutDown();
	}

	UnlockAll();
	DeleteCriticalSection(&criticalSection);
}

void Script::StopAll(bool force)
{
	LockAll();
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		(*it)->Stop(true, force);
	UnlockAll();
}

void Script::PauseAll(void)
{
	LockAll();
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		(*it)->Pause();
	UnlockAll();
}

void Script::ResumeAll(void)
{
	LockAll();
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		(*it)->Resume();
	UnlockAll();
}

void Script::FlushCache(void)
{
	static bool isFlushing = false;
	if(isFlushing || Vars.bDisableCache)
		return;
	LockAll();
	EnterCriticalSection(&Vars.cFlushCacheSection);
	isFlushing = true;
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		if(!(*it)->IsRunning())
			delete (*it);

	isFlushing = false;
	LeaveCriticalSection(&Vars.cFlushCacheSection);
	UnlockAll();
}

ScriptList Script::GetScripts(void)
{
	LockAll();
	ScriptList scripts;
	for(ScriptMap::iterator it = activeScripts.begin(); it != activeScripts.end(); it++)
		scripts.push_back(it->second);

	UnlockAll();
	return scripts;
}

ScriptMap::iterator Script::GetFirstScript(void)
{
	return activeScripts.begin();
}

ScriptMap::iterator Script::GetLastScript(void)
{
	return activeScripts.end();
}

void Script::RegisterScript(Script* script)
{
	if(Vars.bDisableCache)
		return;

	LockAll();
	if(activeScripts.count(script->fileName) < 1)
		activeScripts[script->fileName] = script;
	UnlockAll();
}

void Script::LockAll(void)
{
/*	EnterCriticalSection(&criticalSection);
	isAllLocked = true;
#ifdef _MSVC_DEBUG
	Log("LockAll: entering on thread %d", GetCurrentThreadId());
#endif*/
}

void Script::UnlockAll(void)
{
/*	LeaveCriticalSection(&criticalSection);
	isAllLocked = false;
#ifdef _MSVC_DEBUG
	Log("UnlockAll: leaving on thread %d", GetCurrentThreadId());
#endif*/
}

bool Script::IsAllLocked(void)
{
	return isAllLocked;
}

int Script::GetCount(void)
{
	return activeScripts.size();
}

int Script::GetActiveCount(bool countUnexecuted)
{
	LockAll();
	int count = 0;
	for(ScriptMap::iterator it = activeScripts.begin(); it != activeScripts.end(); it++)
		if((it->second->IsRunning() && !it->second->IsAborted()) ||
			(countUnexecuted && (it->second->GetExecutionCount() == 0)))
			count++;
	UnlockAll();
	return count;
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

	JS_SetContextThread(GetContext());
	JS_BeginRequest(GetContext());

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	JS_ExecuteScript(GetContext(), globalObject, script, &dummy);
	
	JS_GetProperty(GetContext(), globalObject, "main", &main);

	if(JSVAL_IS_FUNCTION(GetContext(), main))
	{
		JS_CallFunctionValue(GetContext(), globalObject, main, 0, NULL, &dummy);
	}
	
	// assume the context has been trampled
	if ((DWORD)JS_GetContextThread(GetContext()) != GetThreadId())
		JS_SetContextThread(GetContext());

	JS_EndRequest(GetContext());
	JS_ClearContextThread(GetContext());

	execCount++;
	Stop();
}

void Script::Pause(void)
{
	AutoLock lock(this);
	if(!IsAborted() && !IsPaused())
		isPaused = true;
}

void Script::Resume(void)
{
	AutoLock lock(this);
	if(!IsAborted() && IsPaused())
		isPaused = false;
}

bool Script::IsPaused(void)
{
	return isPaused;
}

void Script::Stop(bool force, bool reallyForce)
{
	AutoLock lock(this);
	
	// Clear the events/hooks before aborting the script, otherwise we can't clean up all the events in the context
	// what could possibly blow up by cleaning up hooks/events after stopping the script?
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

}

void Script::EnableSingleStep(void)
{
	singleStep = true;
}

void Script::DisableSingleStep(void)
{
	singleStep = false;
}

bool Script::IsSingleStep(void)
{
	return singleStep;
}

bool Script::IsIncluded(const char* file)
{
	char* fname = _strlwr((char*)file);
	StringReplace(fname, '/', '\\');
	return !!includes.count(string(fname));
}

bool Script::Include(const char* file)
{
	AutoLock lock(this);
	char* fname = _strlwr((char*)file);
	StringReplace(fname, '/', '\\');
	// ignore already included, 'in-progress' includes, and self-inclusion
	if(IsIncluded(fname) || !!inProgress.count(string(fname)) || strcmp(fileName, fname) == 0)
		return true;
	bool rval = false;
	JSContext* tmpcx = BuildContext(runtime);
	JS_BeginRequest(tmpcx);
	JS_SetContextPrivate(tmpcx, this);
	JSScript* script = JS_CompileFile(tmpcx, globalObject, fname);
	if(script)
	{
		jsval dummy;
		inProgress[fname] = true;
		rval = !!JS_ExecuteScript(tmpcx, globalObject, script, &dummy);
		JS_DestroyScript(tmpcx, script);
		if(rval)
			includes[fname] = true;
		inProgress.erase(fname);
	}
	JS_EndRequest(tmpcx);
	JS_DestroyContextMaybeGC(tmpcx);
	return rval;
}

bool Script::IsRunning(void)
{
	return context && !(!JS_IsRunning(context) || IsPaused());
}

bool Script::IsAborted()
{
	return isAborted;
}

void Script::Lock(void)
{
/*	lockThreadId = GetCurrentThreadId();
	isLocked = true;
	EnterCriticalSection(&scriptSection);
#ifdef _MSVC_DEBUG
	Log("Lock: entering lock for %s on thread %d", fileName, lockThreadId);
#endif*/
}

void Script::Unlock(void)
{
/*#ifdef _MSVC_DEBUG
	Log("Unlock: leaving lock for %s on thread %d", fileName, lockThreadId);
#endif
	lockThreadId = -1;
	LeaveCriticalSection(&scriptSection);
	isLocked = false;*/
}

bool Script::IsLocked(void)
{
	return isLocked;
}

DWORD Script::LockingThread()
{
	return lockThreadId;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	AutoLock lock(this);
	if(JSVAL_IS_FUNCTION(context, evtFunc))
	{
		AutoRoot* root = new AutoRoot(context, evtFunc);
		root->Take();
		functions[evtName].push_back(root);
	}
}

bool Script::IsRegisteredEvent(const char* evtName, jsval evtFunc)
{
	AutoLock lock(this);
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
		if((*it)->value() == evtFunc)
			return true;

	return false;
}

void Script::UnregisterEvent(const char* evtName, jsval evtFunc)
{
	AutoLock lock(this);
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
}

void Script::ClearEvent(const char* evtName)
{
	AutoLock lock(this);
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		AutoRoot* func = *it;
		func->Release();
		delete func;
	}
	functions[evtName].clear();
}

void Script::ClearAllEvents(void)
{
	AutoLock lock(this);
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
}

JSBool Script::ExecEvent(char* evtName, uintN argc, AutoRoot** argv, jsval* rval)
{
	char msg[50];
	sprintf(msg, "Script::ExecEvent(%s)", evtName);
	CDebug cDbg(msg);

	Pause();

	JSContext* cx = BuildContext(runtime);
	JS_SetContextPrivate(cx, this);
	for(uintN i = 0; i < argc; i++)
		argv[i]->Take();

	if(JS_GetContextThread(context) != GetCurrentThreadId())
		THROW_ERROR(context, globalObject, "Cross-thread attempt to execute an event");

	JS_BeginRequest(cx);
	FunctionList flist = functions[evtName];

	jsval* args = new jsval[argc];
	for(uintN i = 0; i < argc; i++)
		args[i] = argv[i]->value();

	for(FunctionList::iterator it = flist.begin(); it != flist.end(); it++)
		JS_CallFunctionValue(cx, globalObject, (*it)->value(), argc, args, rval);

	delete[] args;
	JS_EndRequest(cx);
	JS_DestroyContext(cx);
	cx = NULL;

	for(uintN i = 0; i < argc; i++)
		argv[i]->Release();

	delete[] argv;

	Resume();

	return JS_TRUE;
}

void Script::ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv)
{
	AutoLock lock(this);
	if(!IsAborted() && !IsPaused() && functions.count(evtName))
	{
		char msg[50];
		sprintf(msg, "Script::ExecEventAsync(%s)", evtName);
		CDebug cDbg(msg);

		for(uintN i = 0; i < argc; i++)
			argv[i]->Take();

		Event* evt = new Event;
		evt->owner = this;
		evt->functions = functions[evtName];
		evt->argc = argc;
		evt->argv = argv;
		evt->context = BuildContext(runtime);
		evt->object = globalObject;
		JS_SetContextPrivate(evt->context, this);

		CreateThread(0, 0, FuncThread, evt, 0, 0);
	}
}

void Script::ExecEventOnAll(char* evtName, uintN argc, AutoRoot** argv)
{
	LockAll();
	jsval dummy;
	ScriptList scripts = GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
		if(!(*it)->IsAborted())
			(*it)->ExecEvent(evtName, argc, argv, &dummy);
	UnlockAll();
}

void Script::ExecEventAsyncOnAll(char* evtName, uintN argc, AutoRoot** argv)
{
	LockAll();
	ScriptList scripts = GetScripts();
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
		if(!(*it)->IsAborted())
			(*it)->ExecEventAsync(evtName, argc, argv);
	UnlockAll();
}

DWORD WINAPI ScriptThread(void* data)
{
	Script* script = (Script*)data;
	if(script)
	{
		script->Run();
		if(Vars.bDisableCache)
			delete script;
	}
	return 0;
}

DWORD WINAPI FuncThread(void* data)
{
	Event* evt = (Event*)data;
	if(!evt)
		return 0;

	// switch the context thread once and only once to this one, since it won't be this thread's context
	JS_ClearContextThread(evt->context);
	JS_SetContextThread(evt->context);	
	JS_BeginRequest(evt->context);

	if(!evt->owner->IsAborted() || !(evt->owner->GetState() == InGame && !GameReady()))
	{
		jsval dummy = JSVAL_VOID;

		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = evt->argv[i]->value();
			JS_AddRoot(evt->context, &args[i]);
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
		JS_EndRequest(evt->context);
	}

	JS_EndRequest(evt->context);
	JS_ClearContextThread(evt->context);
	JS_DestroyContext(evt->context);
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


JSTrapStatus exceptionCallback(JSContext *cx, JSScript *script, jsbytecode *pc, jsval *rval, void *closure)
{
	return JSTRAP_CONTINUE;
}

void* executeCallback(JSContext* cx, JSStackFrame* frame, JSBool before, JSBool* ok, void* closure)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	if(!script)
		return NULL;

	static JSBool dummy;
	dummy = (script->IsSingleStep() ? JS_TRUE : JS_FALSE);
	return dummy ? &dummy : NULL;
}

JSTrapStatus debuggerCallback(JSContext *cx, JSScript *jsscript, jsbytecode *pc, jsval *rval, void *closure)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	if(!script)
		return JSTRAP_CONTINUE;

	script->EnableSingleStep();
	return JSTRAP_CONTINUE;
}

JSBool branchCallback(JSContext* cx, JSScript*)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	bool pause = script->IsPaused();

	jsrefcount depth = JS_SuspendRequest(cx);

	if(pause)
	{
		// assume we have to take the context thread
		// break the locks if necessary
		if(!script->IsLocked())
			script->Lock();
		script->SetPauseState(true);
	}
	while(script->IsPaused())
		Sleep(50);
	if(pause)
	{
		script->SetPauseState(false);
		if(script->LockingThread() == GetCurrentThreadId())
			script->Unlock();
	}
	// assume it was trampled.
	JS_SetContextThread(cx);
	JS_ResumeRequest(cx, depth);
	// there is no need to use up MORE processor time for an if statement when it can be collapsed down.
	// JS_TRUE and JS_FALSE are simply typecasted 1 and 0 respectively.
	return !!!(JSBool)(script->IsAborted() || ((script->GetState() != OutOfGame) && !D2CLIENT_GetPlayerUnit()));
}

JSBool gcCallback(JSContext *cx, JSGCStatus status)
{
	if(status == JSGC_BEGIN)
	{
		if(Vars.bDebug)
			Log("*** ENTERING GC ***");
		if(JS_GetContextThread(cx))
			JS_ClearContextThread(cx);
		JS_SetContextThread(cx);
	}
	else if(status == JSGC_END)
	{
		if(Vars.bDebug)
			Log("*** LEAVING GC ***");
		Script::FlushCache();
	}
	return JS_TRUE;
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	bool warn = JSREPORT_IS_WARNING(report->flags);
	bool isStrict = JSREPORT_IS_STRICT(report->flags);
	const char* type = (warn ? "Warning" : "Error");
	const char* strict = (isStrict ? "Strict " : "");
	const char* filename = (report->filename ? report->filename + strlen(Vars.szScriptPath)+1 : "<unknown>");
	Log("[%s%s] Code (%d) %s/line %d: %s\nLine: %s", strict, type, report->errorNumber, 
				filename, report->lineno, message, report->linebuf);

	Print("[ÿc%d%s%sÿc0 (%d)] %s/line %d: %s", (warn ? 9 : 1), strict, type, report->errorNumber,
					filename, report->lineno, message);

	if(Vars.bQuitOnError && D2CLIENT_GetPlayerUnit() && !JSREPORT_IS_WARNING(report->flags))
		D2CLIENT_ExitGame();
}
