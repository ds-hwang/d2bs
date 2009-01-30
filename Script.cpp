#include "Script.h"
#include <cstring>
#include <cstdio>
#include "windows.h"
#include "JSGlobalFuncs.h"
#include "js32.h"
#include "D2Ptrs.h"

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
LPCRITICAL_SECTION Script::criticalSection = NULL;
bool Script::isAllLocked = false;

Script* Script::CompileFile(const char* file, ScriptState state, bool recompile)
{
	try {
		if(recompile && activeScripts.count(file)) {
			delete activeScripts[file];
			return new Script(file, state);
		}
		if(activeScripts.count(file) > 0 && !activeScripts[file]->IsRunning())
			return activeScripts[file];
		return new Script(file, state);
	} catch(std::exception e) {
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

Script* Script::CompileCommand(const char* command)
{
	try {
		if(activeScripts.count(command) > 0 && !activeScripts[command]->IsRunning())
			return activeScripts[command];
		return new Script(command, Command);
	} catch(std::exception e) {
		Print(const_cast<char*>(e.what()));
		return NULL;
	}
}

Script::Script(const char* file, ScriptState state) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), fileName(NULL),
			execCount(0), isAborted(false), isPaused(false), singleStep(false), scriptState(state),
			threadHandle(NULL), threadId(0)
{
	if(scriptState != Command && _access(file, 0) != 0)
		throw std::exception("File not found");

	scriptSection = new CRITICAL_SECTION;
	InitializeCriticalSection(scriptSection);
	Lock();
	fileName = _strdup(file);
	context = JS_NewContext(runtime, 0x2000);
	if(!context)
		throw std::exception("Couldn't create the context");
	JS_BeginRequest(context);
	try {
		JS_SetContextPrivate(context, this);
		JS_SetErrorReporter(context, reportError);
		JS_SetBranchCallback(context, branchCallback);
		JS_SetOptions(context, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML|JSOPTION_NATIVE_BRANCH_CALLBACK);
		JS_SetVersion(context, JSVERSION_1_7);

		globalObject = JS_NewObject(context, &global_obj, NULL, NULL);
		if(!globalObject)
			throw std::exception("Couldn't create the global object");

		JS_InitStandardClasses(context, globalObject);
		JS_DefineFunctions(context, globalObject, global_funcs);

		InitClass(&file_class_ex.base, file_methods, file_props, file_s_methods, NULL);
		InitClass(&filetools_class, NULL, NULL, filetools_s_methods, NULL);
		InitClass(&sqlite_db_ex.base, sqlite_methods, sqlite_props, NULL, NULL);
		InitClass(&sandbox_class, sandbox_methods, NULL, NULL, NULL);
		InitClass(&frame_class, frame_methods, frame_props, NULL, NULL);
		InitClass(&box_class, box_methods, box_props, NULL, NULL);
		InitClass(&line_class, line_methods, line_props, NULL, NULL);
		InitClass(&text_class, text_methods, text_props, NULL, NULL);
		InitClass(&image_class, image_methods, image_props, NULL, NULL);

		myUnit* lpUnit = new myUnit;

		if(!lpUnit)
			throw std::exception("Couldn't define the 'me' object!");

		memset(lpUnit, NULL, sizeof(myUnit));

		lpUnit->dwMode = -1;
		lpUnit->dwClassId = -1;
		lpUnit->dwType = UNIT_PLAYER;
		lpUnit->dwUnitId = D2CLIENT_GetPlayerUnit() ? D2CLIENT_GetPlayerUnit()->dwUnitId : NULL;
		lpUnit->_dwPrivateType = PRIVATE_UNIT;

		meObject = BuildObject(context, &unit_class, unit_methods, me_props, lpUnit);
		JS_AddRoot(context, &meObject);
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
			script = JS_CompileScript(context, globalObject, file, strlen(file), "Command Line", 0);
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
		RegisterScript(this);
		Unlock();
	} catch(...) {
		Unlock();
		DeleteCriticalSection(scriptSection);
		delete scriptSection;
		JS_EndRequest(context);
		JS_DestroyContext(context);
		throw;
	}
}

Script::~Script(void)
{
	Stop(true, true);
	Lock();
	activeScripts.erase(fileName);

	JS_SetContextThread(context);
	JS_BeginRequest(context);

	JS_RemoveRoot(context, &meObject);
	JS_RemoveRoot(context, &scriptObject);

	JS_EndRequest(context);
	JS_DestroyContextNoGC(context);

	context = NULL;
	scriptObject = NULL;
	globalObject = NULL;
	script = NULL;
	delete[] fileName;

	includes.clear();
	functions.clear();
	if(threadHandle)
		CloseHandle(threadHandle);
	Unlock();
	DeleteCriticalSection(scriptSection);
	delete scriptSection;
}

void Script::InitClass(JSClass* classp, JSFunctionSpec* methods, JSPropertySpec* props,
					   JSFunctionSpec* s_methods, JSPropertySpec* s_props)
{
	if(!JS_InitClass(context, globalObject, NULL, classp, classp->construct, 0,
		props, methods, s_props, s_methods))
		throw "Couldn't initialize the class";
}

void Script::DefineConstant(const char* name, int value)
{
	if(!JS_DefineProperty(context, globalObject, name, INT_TO_JSVAL(value), NULL, NULL, JSPROP_CONSTANT))
		throw "Couldn't initialize the constant";
}

void Script::Startup(void)
{
	if(!runtime)
	{
		runtime = JS_NewRuntime(0x800000);
		JS_SetGCCallbackRT(runtime, gcCallback);
		//JS_SetThrowHook(runtime, exceptionCallback, NULL);
		//JS_SetCallHook(runtime, executeCallback, NULL);
		//JS_SetExecuteHook(runtime, executeCallback, NULL);
		//JS_SetDebuggerHandler(runtime, debuggerCallback, NULL);
		criticalSection = new CRITICAL_SECTION;
		InitializeCriticalSection(criticalSection);
	}
}

void Script::Shutdown(void)
{
	Script::StopAll(true);
	LockAll();
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		delete (*it);

	activeScripts.clear();

	if(runtime)
	{
		JS_DestroyRuntime(runtime);
		JS_ShutDown();
	}

	UnlockAll();
	DeleteCriticalSection(criticalSection);
	delete criticalSection;
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
	LockAll();
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		if(!(*it)->IsRunning())
			delete (*it);

	UnlockAll();
}

ScriptList Script::GetScripts(void)
{
	ScriptList scripts;
	for(ScriptMap::iterator it = activeScripts.begin(); it != activeScripts.end(); it++)
		scripts.push_back(it->second);
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
	LockAll();
	if(activeScripts.count(script->fileName) < 1)
		activeScripts[script->fileName] = script;
	UnlockAll();
}

void Script::LockAll(void)
{
	EnterCriticalSection(criticalSection);
	isAllLocked = true;
}

void Script::UnlockAll(void)
{
	LeaveCriticalSection(criticalSection);
	isAllLocked = false;
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
	isAborted = false;
	JS_SetContextThread(context);
	JS_BeginRequest(context);
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
	threadId = GetCurrentThreadId();

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	JS_AddRoot(context, &main);
	JS_ExecuteScript(context, globalObject, script, &dummy);
	if(JS_GetProperty(context, globalObject, "main", &main) && JSVAL_IS_FUNCTION(context, main))
	{
		JS_SetContextThread(context);
		JS_CallFunctionValue(context, globalObject, main, 0, NULL, &dummy);
	}

	JS_RemoveRoot(context, &main);
	// the context's thread most likely was trampled on by now, reset it
	JS_SetContextThread(context);
	JS_EndRequest(context);
	JS_ClearContextThread(context);
	execCount++;
	Stop();
}

void Script::Pause(void)
{
	Lock();
	if(!IsAborted() && threadId != GetCurrentThreadId() && !IsPaused() && threadHandle)
	{
		SuspendThread(threadHandle);
		isPaused = true;
	}
	Unlock();
}

void Script::Resume(void)
{
	Lock();
	if(!IsAborted() && threadId != GetCurrentThreadId() && IsPaused() && threadHandle)
	{
		ResumeThread(threadHandle);
		isPaused = false;
	}
	Unlock();
}

bool Script::IsPaused(void)
{
	return isPaused;
}

void Script::Stop(bool force, bool reallyForce)
{
	AutoLock lock(this);

	isAborted = true;
	isPaused = false;

	ClearAllEvents();
	Genhook::Clean(this);

	int maxCount = reallyForce ? 2 : 30;
	for(int i = 0; IsRunning(); i++)
	{
		if(i >= maxCount)
			break;
		Sleep(500);
	}

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
	return !!includes.count(string(file));
}

bool Script::Include(const char* file)
{
	if(IsIncluded(file))
		return true;
	Lock();
	bool rval = false;
	JSScript* script = JS_CompileFile(context, globalObject, file);
	if(script)
	{
		jsval dummy;
		rval = !!JS_ExecuteScript(context, globalObject, script, &dummy);
		JS_DestroyScript(context, script);
		if(rval)
			includes[file] = true;
	}
	Unlock();
	return rval;
}

bool Script::IsRunning(void)
{
	return !(!JS_IsRunning(context) || isPaused);
}

bool Script::IsAborted()
{
	return isAborted;
}

void Script::Lock(void)
{
	EnterCriticalSection(scriptSection);
	isLocked = true;
}

void Script::Unlock(void)
{
	LeaveCriticalSection(scriptSection);
	isLocked = false;
}

bool Script::IsLocked(void)
{
	return isLocked;
}

void Script::RegisterEvent(const char* evtName, jsval evtFunc)
{
	if(JSVAL_IS_FUNCTION(context, evtFunc))
	{
		AutoRoot* root = new AutoRoot(context, evtFunc);
		root->Take();
		functions[evtName].push_back(root);
	}
}

bool Script::IsRegisteredEvent( const char* evtName, jsval evtFunc )
{
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
		if((*it)->value() == evtFunc)
			return true;

	return false;
}

void Script::UnregisterEvent(const char* evtName, jsval evtFunc)
{
	Lock();
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
	Unlock();
}

void Script::ClearEvent(const char* evtName)
{
	Lock();
	for(FunctionList::iterator it = functions[evtName].begin(); it != functions[evtName].end(); it++)
	{
		AutoRoot* func = *it;
		func->Release();
		delete func;
	}
	functions[evtName].clear();
	Unlock();
}

void Script::ClearAllEvents(void)
{
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
}

JSBool Script::ExecEvent(char* evtName, uintN argc, AutoRoot** argv, jsval* rval)
{
	char msg[50];
	sprintf(msg, "Script::ExecEvent(%s)", evtName);
	CDebug cDbg(msg);

	Pause();
	Lock();

	for(uintN i = 0; i < argc; i++)
		argv[i]->Take();

	if(JS_GetContextThread(context) != GetCurrentThreadId())
		THROW_ERROR(context, globalObject, "Cross-thread attempt to execute an event");

	JS_BeginRequest(context);
	FunctionList flist = functions[evtName];

	jsval* args = new jsval[argc];
	for(uintN i = 0; i < argc; i++)
		args[i] = argv[i]->value();

	for(FunctionList::iterator it = flist.begin(); it != flist.end(); it++)
		JS_CallFunctionValue(context, globalObject, (*it)->value(), argc, args, rval);

	delete[] args;
	JS_EndRequest(context);

	for(uintN i = 0; i < argc; i++)
		argv[i]->Release();

	delete[] argv;

	Unlock();
	Resume();

	return JS_TRUE;
}

void Script::ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv)
{
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
		evt->context = context;
		evt->object = globalObject;

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
	((Script*)data)->Run();
	return 0;
}

DWORD WINAPI FuncThread(void* data)
{
	Event* evt = (Event*)data;
	evt->owner->Pause();
	evt->owner->Lock();
	if(!evt->owner->IsAborted() || !(evt->owner->GetState() == InGame && !GameReady()))
	{
		jsval dummy;
		// switch the context thread to this one
		JS_SetContextThread(evt->context);
		JS_BeginRequest(evt->context);

		jsval* args = new jsval[evt->argc];
		for(uintN i = 0; i < evt->argc; i++)
		{
			args[i] = evt->argv[i]->value();
			JS_AddRoot(evt->context, &args[i]);
		}

		for(FunctionList::iterator it = evt->functions.begin(); it != evt->functions.end(); it++)
			JS_CallFunctionValue(evt->context, evt->object, (*it)->value(), evt->argc, args, &dummy);

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(evt->context, &args[i]);
		delete[] args;

		// assume that the caller stole the context thread
		JS_SetContextThread(evt->context);
		JS_EndRequest(evt->context);
	}
	evt->owner->Unlock();
	evt->owner->Resume();

	// assume we have to clean up both the event and the args, and release autorooted vars
	for(uintN i = 0; i < evt->argc; i++)
		evt->argv[i]->Release();
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

	if(script->IsAborted())
		return JS_FALSE;

	if((script->GetState() != OutOfGame) && !D2CLIENT_GetPlayerUnit())
		return JS_FALSE;

	return JS_TRUE;
}

JSBool gcCallback(JSContext *cx, JSGCStatus status)
{
	if(status == JSGC_BEGIN)
	{
		Script::PauseAll();
		Script::LockAll();
		JS_SetContextThread(cx);
	}
	else if(status == JSGC_END)
	{
		Script::UnlockAll();
		Script::ResumeAll();
	}
	return JS_TRUE;
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	char msg[1024];
	char* type = (JSREPORT_IS_WARNING(report->flags) ? "ÿc9Warning" : "ÿc1Error");
	sprintf(msg, "[%sÿc0] %s/line %d: %s", type, (report->filename+strlen(Vars.szScriptPath)+1), report->lineno, message);
	Log(msg);
	// all potential cases are handled inside Print now
	Print(msg);
	if(Vars.bQuitOnError && D2CLIENT_GetPlayerUnit() && !JSREPORT_IS_WARNING(report->flags))
		D2CLIENT_ExitGame();
}
