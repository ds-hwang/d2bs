#pragma once

#include <string>
#include <map>
#include <list>
#include <windows.h>

#include "js32.h"
#include "AutoRoot.h"

enum ScriptExecState {
	Unexecuted,
	Running,
	Paused,
	Stopped
};

enum ScriptType {
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

// TODO: replace this with a std::set and use that
// to ensure include compliance, faster/less code
typedef std::map<std::string, bool> IncludeList;
typedef std::list<AutoRoot*> FunctionList;
typedef std::map<std::string, FunctionList> FunctionMap;
typedef std::list<Script*> ScriptList;

struct Event {
	Script* owner;
	FunctionList functions;
	AutoRoot** argv;
	uintN argc;
	DWORD dwProduce;
	DWORD dwConsume;
};

class Script
{
protected:
	std::string fileName;
	int execCount;
	ScriptType scriptType;
	ScriptExecState scriptState;
	JSContext* context;
	JSScript* script;

	JSObject *globalObject, *scriptObject;
	bool isLocked;

	IncludeList includes, inProgress;
	FunctionMap functions;
	HANDLE threadHandle;
	DWORD threadId;
	CRITICAL_SECTION lock;

private:
	Script(const char* file,ScriptType scripttype);
	Script(const Script&);
	Script& operator=(const Script&);
	~Script(void);

public:
	friend class ScriptEngine;

	void Run(void);
	void Pause(void);
	void Resume(void);
	void Stop(void);

	ScriptExecState GetScriptState(void) { return scriptState; }
	void SetScriptState(ScriptExecState state);
	ScriptType GetScriptType(void) { return scriptType; }

	const char* GetFilename(void);
	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
	JSObject* GetScriptObject(void) { return scriptObject; }
	int GetExecutionCount(void);
	DWORD GetThreadId(void);

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
