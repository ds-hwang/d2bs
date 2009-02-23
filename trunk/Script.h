#pragma once

#include <windows.h>
#include "js32.h"
#include <string>
#include <map>
#include <list>

enum ScriptState {
	InGame,
	OutOfGame,
	Command
};

static JSClass global_obj = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

class Script;

class AutoRoot
{
private:
	JSContext* context;
	jsval var;
	uint count;

	AutoRoot(const AutoRoot&);
	AutoRoot& operator=(const AutoRoot&);
public:
	AutoRoot() {}
	AutoRoot(JSContext* cx, jsval var);
	AutoRoot::~AutoRoot();
	void AutoRoot::Take();
	void AutoRoot::Release();
	jsval AutoRoot::value();
	bool operator==(AutoRoot& other);
};

typedef std::map<std::string, bool> IncludeList;
typedef std::list<AutoRoot*> FunctionList;
typedef std::map<std::string, FunctionList> FunctionMap;
typedef std::list<Script*> ScriptList;
typedef std::map<std::string, Script*> ScriptMap;

struct Event {
	Script* owner;
	JSContext* context;
	JSObject* object;
	FunctionList functions;
	AutoRoot** argv;
	uintN argc;
};

class Script
{
private:
	static JSRuntime* runtime;
	static ScriptMap activeScripts;
	static CRITICAL_SECTION criticalSection;
	static bool isAllLocked;

	char* fileName;
	int execCount;
	ScriptState scriptState;
	JSContext* context;
	JSScript* script;

	JSObject *globalObject, *scriptObject, *meObject;
	bool isLocked, isPaused, isReallyPaused, isAborted, singleStep;

	IncludeList includes, inProgress;
	FunctionMap functions;
	CRITICAL_SECTION scriptSection;
	HANDLE threadHandle;
	DWORD threadId, lockThreadId;

	void InitClass(JSClass* classp, JSFunctionSpec* methods, JSPropertySpec* props,
					JSFunctionSpec* s_methods, JSPropertySpec* s_props);
	void DefineConstant(const char* name, int value);

	static void RegisterScript(Script* script);

	Script(const char* file, ScriptState state);
	~Script(void);
	Script(const Script&);
	Script& operator=(const Script&);
public:
	static Script* CompileFile(const char* file, ScriptState state, bool recompile = false);
	static Script* CompileCommand(const char* command);

	static JSRuntime* GetRuntime(void) { return runtime; }

	static void Startup(void);
	static void Shutdown(void);
	static void FlushCache(void);

	static ScriptList GetScripts(void);
	static ScriptMap::iterator GetFirstScript(void);
	static ScriptMap::iterator GetLastScript(void);
	static int GetCount(void);
	static int GetActiveCount(bool countUnexecuted = false);

	static void StopAll(bool force = false);
	static void PauseAll(void);
	static void ResumeAll(void);

	static void LockAll(void);
	static void UnlockAll(void);
	static bool IsAllLocked(void);

	void Run(void);
	void Pause(void);
	void Resume(void);
	bool IsPaused(void);
	void SetPauseState(bool reallyPaused) { isReallyPaused = reallyPaused; }
	bool IsReallyPaused(void) { return isReallyPaused; }
	void Stop(bool force = false, bool reallyForce = false);

	void EnableSingleStep(void);
	void DisableSingleStep(void);
	bool IsSingleStep(void);

	char* GetFilename(void) { return fileName; }
	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
	JSObject* GetScriptObject(void) { return scriptObject; }
	ScriptState GetState(void) { return scriptState; }
	int GetExecutionCount(void);
	DWORD GetThreadId(void);

	void Lock(void);
	void Unlock(void);
	bool IsLocked(void);
	DWORD LockingThread(void);

	bool IsRunning(void);
	bool IsAborted(void);

	bool IsIncluded(const char* file);
	bool Include(const char* file);

	void RegisterEvent(const char* evtName, jsval evtFunc);
	bool IsRegisteredEvent(const char* evtName, jsval evtFunc);
	void UnregisterEvent(const char* evtName, jsval evtFunc);
	void ClearEvent(const char* evtName);
	void ClearAllEvents(void);

	JSBool ExecEvent(char* evtName, uintN argc, AutoRoot** argv, jsval* rval);
	void ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv);
	static void ExecEventOnAll(char* evtName, uintN argc, AutoRoot** argv);
	static void ExecEventAsyncOnAll(char* evtName, uintN argc, AutoRoot** argv);
};

class AutoLock
{
private:
	Script* script;
public:
	AutoLock(Script* s) { script = s; s->Lock(); }
	~AutoLock() { script->Unlock(); }
};

DWORD WINAPI ScriptThread(void* data);
DWORD WINAPI FuncThread(void* data);

JSBool watchHandler(JSContext* cx, JSObject* obj, jsval id, jsval old, jsval* newval, void* closure);
JSTrapStatus debuggerCallback(JSContext* cx, JSScript* script, jsbytecode* pc, jsval* rval, void* closure);
JSTrapStatus exceptionCallback(JSContext* cx, JSScript* script, jsbytecode* pc, jsval* rval, void* closure);
void* executeCallback(JSContext* cx, JSStackFrame* frame, JSBool before, JSBool* ok, void* closure);
JSBool branchCallback(JSContext* cx, JSScript* script);
JSBool gcCallback(JSContext* cx, JSGCStatus status);
void reportError(JSContext *cx, const char *message, JSErrorReport *report);
