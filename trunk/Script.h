#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <vector>

#include "js32.h"
#include "AutoRoot.h"
#include "JSUnit.h"

enum ScriptState {
	InGame,
	OutOfGame,
	Command
};

enum ArgType {
	String,
	UnsignedInt,
	SignedInt,
	UnsignedShort,
	Double,
	Boolean,
	JSVal
};

typedef __int64 QWORD;

typedef std::pair<QWORD, ArgType> EventArg;
typedef std::vector<EventArg> ArgList;

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
	FunctionList functions;
	Script* owner;
	JSObject* object;
	char format[10];
	ArgList* args;
};

class Script
{
private:
	std::string fileName;
	int execCount;
	ScriptState scriptState;
	JSContext* context;
	JSScript* script;
	myUnit* me;

	JSObject *globalObject, *scriptObject;
	bool isLocked, isPaused, isReallyPaused, isAborted;

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

	const char* GetFilename(void) { const char* file = _strdup(fileName.c_str()); return file; }
	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
	JSObject* GetScriptObject(void) { return scriptObject; }
	ScriptState GetState(void) { return scriptState; }
	int GetExecutionCount(void);
	DWORD GetThreadId(void);
	// UGLY HACK to fix up the player gid on game join for cached scripts/oog scripts
	void UpdatePlayerGid(void);

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

	void ExecEvent(const char* evtName, const char* format, uintN argc, void* argv);
	void ExecEventAsync(const char* evtName, const char* format, ArgList* args);
};

DWORD WINAPI ScriptThread(void* data);
void __cdecl FuncThread(void* data);
