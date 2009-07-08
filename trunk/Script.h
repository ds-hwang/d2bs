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

// TODO: replace this with a std::set and use that
// to ensure include compliance, faster/less code
typedef std::map<std::string, bool> IncludeList;
typedef std::list<AutoRoot*> FunctionList;
typedef std::map<std::string, FunctionList> FunctionMap;
typedef std::list<Script*> ScriptList;

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
	std::string fileName;
	int execCount;
	ScriptState scriptState;
	JSContext* context;
	JSScript* script;

	JSObject *globalObject, *scriptObject, *meObject;
	bool isLocked, isPaused, isReallyPaused, isAborted, singleStep;

	IncludeList includes, inProgress;
	FunctionMap functions;
	HANDLE threadHandle;
	DWORD threadId, lockThreadId;
	CRITICAL_SECTION lock;

	void InitClass(JSClass* classp, JSFunctionSpec* methods, JSPropertySpec* props,
					JSFunctionSpec* s_methods, JSPropertySpec* s_props);
	void DefineConstant(const char* name, int value);

	Script(const char* file, ScriptState state);
	Script(const Script&);
	Script& operator=(const Script&);
	~Script(void);
public:
	friend class ScriptEngine;

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

	char* GetFilename(void) { return scriptState == Command ? "<Command Line>" : fileName.c_str(); }
	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
	JSObject* GetScriptObject(void) { return scriptObject; }
	ScriptState GetState(void) { return scriptState; }
	int GetExecutionCount(void);
	DWORD GetThreadId(void);

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
};

DWORD WINAPI ScriptThread(void* data);
DWORD WINAPI FuncThread(void* data);
