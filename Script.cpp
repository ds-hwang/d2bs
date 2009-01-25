#include "Script.h"
#include <cstring>
#include <cstdio>
#include "windows.h"
#include "JSGlobalFuncs.h"
#include "js32.h"
#include "D2Ptrs.h"

using namespace std;

AutoRoot::AutoRoot(JSContext* cx, jsval nvar) :	context(cx), var(nvar), count(1) { JS_AddRoot(cx, &var); }
AutoRoot::~AutoRoot()
{
	if(!!(count--))
	{
		fprintf(stderr, "AutoRoot failed: Count is still %i, but the root is being destroyed", count);
		DebugBreak();
		exit(3);
	}
	JS_RemoveRoot(context, &var);
}
void AutoRoot::Take() { count++; JS_AddRoot(context, &var); }
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
	if(recompile && activeScripts.count(file)) {
		delete activeScripts[file];
		return new Script(file, state);
	}
	if(activeScripts.count(file) > 0 && !activeScripts[file]->IsRunning())
		return activeScripts[file];
	return new Script(file, state);
}

Script* Script::CompileCommand(const char* command)
{
	if(activeScripts.count(command) > 0 && !activeScripts[command]->IsRunning())
		return activeScripts[command];
	return new Script(command, Command);
}

Script::Script(const char* file, ScriptState state) :
			context(NULL), globalObject(NULL), scriptObject(NULL), script(NULL), fileName(NULL),
			execCount(0), isAborted(false), scriptState(state), threadHandle(NULL)
{
	if(scriptState != Command && _access(file, 0) != 0)
		throw "File not found";

	scriptSection = new CRITICAL_SECTION;
	InitializeCriticalSection(scriptSection);
	Lock();
	fileName = _strdup(file);
	context = JS_NewContext(runtime, 0x2000);
	if(!context)
		throw "Couldn't create the context";
	JS_BeginRequest(context);
	try {
		JS_SetContextPrivate(context, this);
		JS_SetErrorReporter(context, reportError);
		JS_SetBranchCallback(context, branchCallback);
		JS_SetOptions(context, JSOPTION_STRICT|JSOPTION_VAROBJFIX|JSOPTION_XML|JSOPTION_NATIVE_BRANCH_CALLBACK);
		JS_SetVersion(context, JSVERSION_1_7);

		globalObject = JS_NewObject(context, &global_obj, NULL, NULL);
		if(!globalObject)
			throw "Couldn't create the global object";

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
			throw "Couldn't define the 'me' object!";

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
			throw "Couldn't compile the script";

		scriptObject = JS_NewScriptObject(context, script);
		if(!scriptObject)
			throw "Couldn't create the script object";

		JS_AddNamedRoot(context, &meObject, "me object");
		JS_AddNamedRoot(context, &scriptObject, "script object");
		JS_EndRequest(context);
		RegisterScript(this);
		Unlock();
	} catch(const char*) {
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
	Lock();
	Stop();
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
		// TODO: Look into locking the GC on one thread
		criticalSection = new CRITICAL_SECTION;
		InitializeCriticalSection(criticalSection);
	}
}

void Script::Shutdown(void)
{
	LockAll();
	Vars.bAbortScripts = TRUE;
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
		delete *it;

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

void Script::FlushCache(void)
{
	LockAll();
	ScriptList list = GetScripts();
	for(ScriptList::iterator it = list.begin(); it != list.end(); it++)
	{
		if(!(*it)->IsRunning())
		{
			activeScripts.erase((*it)->GetFilename());
			delete (*it);
		}
	}
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

void Script::Run(void)
{
	JS_SetContextThread(context);
	JS_BeginRequest(context);
	HANDLE pseudoHandle = GetCurrentThread();
	DuplicateHandle(GetCurrentProcess(), pseudoHandle, GetCurrentProcess(), &threadHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);

	jsval main = JSVAL_VOID, dummy = JSVAL_VOID;
	JS_AddRoot(context, &main);
	JS_ExecuteScript(context, globalObject, script, &dummy);
	if(JS_GetProperty(context, globalObject, "main", &main) && JSVAL_IS_FUNCTION(main))
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
	if(!IsPaused() && threadHandle)
		SuspendThread(threadHandle);
	isPaused = true;
}

void Script::Resume(void)
{
	if(IsPaused() && threadHandle)
		ResumeThread(threadHandle);
	isPaused = false;
}

bool Script::IsPaused(void)
{
	return isPaused;
}

void Script::Stop(void)
{
	isAborted = true;
	isPaused = false;
	while(IsRunning())
		Sleep(500);

	TerminateThread(threadHandle, 0);
	CloseHandle(threadHandle);
	threadHandle = NULL;

	ClearAllEvents();
	Genhook::Clean(this);
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
	JS_BeginRequest(context);
	JSScript* script = JS_CompileFile(context, globalObject, file);
	if(script)
	{
		jsval dummy;
		rval = !!JS_ExecuteScript(context, globalObject, script, &dummy);
		JS_DestroyScript(context, script);
		JS_EndRequest(context);
		if(rval)
			includes[file] = true;
	}
	Unlock();
	return rval;
}

bool Script::IsRunning(void)
{
	return !isPaused && !!JS_IsRunning(context);
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
	if(JSVAL_IS_FUNCTION(evtFunc))
	{
		AutoRoot* root = new AutoRoot(context, evtFunc);
		root->Take();
		functions[evtName].push_back(root);
	}
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
	Lock();
	for(FunctionMap::iterator it = functions.begin(); it != functions.end(); it++)
		ClearEvent(it->first.c_str());
	Unlock();
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
	if(!IsAborted() && functions.count(evtName))
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
		{
			JS_SetContextThread(evt->context);
			JS_CallFunctionValue(evt->context, evt->object, (*it)->value(), evt->argc, args, &dummy);
			JS_ClearContextThread(evt->context);
		}

		for(uintN i = 0; i < evt->argc; i++)
			JS_RemoveRoot(evt->context, &args[i]);
		delete[] args;

		// assume that the caller stole the context thread
		JS_SetContextThread(evt->context);
		JS_EndRequest(evt->context);
		JS_ClearContextThread(evt->context);
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

JSBool branchCallback(JSContext* cx, JSScript*)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);

	if(Vars.bAbortScripts || script->IsAborted() ||
		((script->GetState() != OutOfGame) && !D2CLIENT_GetPlayerUnit()))
		return JS_FALSE;

	// disabled until I figure out how to fix the random GC crashes
	/*static uint32 branchCount = 0;
	script->Lock();
	// every 255 branches, yield the request to allow object sharing
	// this crashes when the context thread is outside of the current thread
	if((branchCount % 0xff) == 1) {
		JS_YieldRequest(cx);
	// every 4095 branches, check to see if we should GC
	} else if((branchCount % 0xfff) == 1) {
		jsrefcount depth = JS_SuspendRequest(cx);
		JS_MaybeGC(cx);
		JS_ResumeRequest(cx, depth);
	// every 65535 branches, force a GC
	} else if((branchCount % 0xffff) == 1) {
		jsrefcount depth = JS_SuspendRequest(cx);
		JS_GC(cx);
		JS_ResumeRequest(cx, depth);
	}
	script->Unlock();
	branchCount++;*/

	return JS_TRUE;
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	char msg[1024];
	char* type = (JSREPORT_IS_WARNING(report->flags) ? "Warning" : "Error");
	sprintf(msg, "[%s] %s@%d: %s", type, report->filename, report->lineno, message);
	Log(msg);
	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(script && script->GetState() == InGame || script->GetState() == Command)
	{
		if(strlen(msg) <= 200)
			Print(msg);
		else
			Print("%s logged to D2BS.log!", type);
	} else
		MessageBox(0, msg, 0, 0);
	if(Vars.bQuitOnError && D2CLIENT_GetPlayerUnit() && !JSREPORT_IS_WARNING(report->flags))
		D2CLIENT_ExitGame();
}
