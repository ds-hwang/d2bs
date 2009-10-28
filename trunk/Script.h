#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <list>

#include "js32.h"

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
	jsval var;
	uint count;

	AutoRoot(const AutoRoot&);
	AutoRoot& operator=(const AutoRoot&);
public:
	AutoRoot() {}
	AutoRoot(jsval var);
	~AutoRoot();
	void Take();
	void Release();
	jsval value();
	jsval operator* ();
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

	JSObject *globalObject, *scriptObject;
	bool isLocked, isPaused, isReallyPaused, isAborted, singleStep;

	IncludeList includes, inProgress;
	FunctionMap functions;
	HANDLE threadHandle;
	DWORD threadId;
	CRITICAL_SECTION lock;

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

	bool IsListenerRegistered(const char* evtName);
	void RegisterEvent(const char* evtName, jsval evtFunc);
	bool IsRegisteredEvent(const char* evtName, jsval evtFunc);
	void UnregisterEvent(const char* evtName, jsval evtFunc);
	void ClearEvent(const char* evtName);
	void ClearAllEvents(void);

	void ExecEventAsync(char* evtName, uintN argc, AutoRoot** argv);

};

DWORD WINAPI ScriptThread(void* data);
DWORD WINAPI FuncThread(void* data);
