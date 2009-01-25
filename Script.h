#pragma once

#include "windows.h"
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
	static LPCRITICAL_SECTION criticalSection;
	static bool isAllLocked;

	char* fileName;
	int execCount;
	bool isAborted;
	ScriptState scriptState;
	JSContext* context;
	JSScript* script;
	JSObject* globalObject;
	JSObject* scriptObject;
	JSObject* meObject;
	IncludeList includes;
	FunctionMap functions;
	LPCRITICAL_SECTION scriptSection;
	bool isLocked, isPaused;
	HANDLE threadHandle;

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
	static void Startup(void);
	static void Shutdown(void);
	static void FlushCache(void);
	static ScriptList GetScripts(void);
	static ScriptMap::iterator GetFirstScript(void);
	static ScriptMap::iterator GetLastScript(void);

	static void LockAll(void);
	static void UnlockAll(void);
	static bool IsAllLocked(void);

	void Run(void);
	void Pause(void);
	void Resume(void);
	bool IsPaused(void);
	void Stop(void);

	static JSRuntime* GetRuntime(void) { return runtime; }
	char* GetFilename(void) { return fileName; }
	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
	JSObject* GetScriptObject(void) { return scriptObject; }
	ScriptState GetState(void) { return scriptState; }
	static int GetCount(void);
	static int GetActiveCount(bool countUnexecuted = false);
	int GetExecutionCount(void);

	void Lock(void);
	void Unlock(void);
	bool IsLocked(void);

	bool IsRunning(void);
	bool IsAborted(void);

	bool IsIncluded(const char* file);
	bool Include(const char* file);

	void RegisterEvent(const char* evtName, jsval evtFunc);
	void UnregisterEvent(const char* evtName, jsval evtFunc);
	void ClearEvent(const char* evtName);
	void ClearAllEvents(void);

	JSBool ExecEvent(char* evtName, uintN argc, AutoRoot** argv, jsval* rval);
	void ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv);
	static void ExecEventOnAll(char* evtName, uintN argc, AutoRoot** argv);
	static void ExecEventAsyncOnAll(char* evtName, uintN argc, AutoRoot** argv);
};

DWORD WINAPI ScriptThread(void* data);
DWORD WINAPI FuncThread(void* data);
JSBool branchCallback(JSContext* cx, JSScript* script);
void reportError(JSContext *cx, const char *message, JSErrorReport *report);
