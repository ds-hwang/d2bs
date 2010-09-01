#pragma once

#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <vector>

#include <iostream>

#include "js32.h"
#include "AutoRoot.h"

//#include "D2BS.h"

enum ScriptExecState {
	ScriptStateCreation,
	ScriptStateRunning,
	ScriptStatePaused,
	ScriptStateAborted,
	ScriptStateAborting,
	ScriptStateUnknown
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
	std::string fileName, shortFileName;
	int execCount;
	ScriptExecState scriptExecState;
	JSContext* context;
	JSScript* script;

	JSObject *globalObject, *scriptObject;

	IncludeList includes, inProgress;
	FunctionMap functions;
	HANDLE threadHandle, trapHandle;
	DWORD threadId;
	CRITICAL_SECTION lock;

	char* trapArg;
	bool trapStep;

	Script(std::string file);
	Script(const Script&);
	Script& operator=(const Script&);
	~Script(void);

public:
	friend class ScriptEngine;

	ScriptExecState GetExecState();
	void Run(void);
	void Pause(void);
	void Resume(void);
	void Stop(bool force = false);

	const std::string& GetFilename(void) { return fileName; }
	const std::string GetShortFilename(void) { return shortFileName; }
	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
	JSObject* GetScriptObject(void) { return scriptObject; }
	JSScript* GetJSScript(void) { return script; }
	int GetExecutionCount(void);
	DWORD GetThreadId(void);

	bool GetTrapStatus(char** arg) {
		WaitForSingleObject(trapHandle, INFINITE);
		bool step = trapStep;
		*arg = _strdup(trapArg);
		trapArg = NULL; trapStep = false;
		return step;
	}
	void SetTrapStatus(bool step, const char* arg) {
		trapStep = step;
		free(trapArg); trapArg = _strdup(arg);
		PulseEvent(trapHandle);
	}

	bool IsIncluded(const std::string &file);
	bool Include(const std::string &file);

	bool IsListenerRegistered(const char* evtName);
	void RegisterEvent(const char* evtName, jsval evtFunc);
	bool IsRegisteredEvent(const char* evtName, jsval evtFunc);
	void UnregisterEvent(const char* evtName, jsval evtFunc);
	void ClearEvent(const char* evtName);
	void ClearAllEvents(void);

// TODO: clean this up and make it either directly call or match SpawnEvent
#if 0
	void ExecEvent(const char* evtName, const char* format, uintN argc, void* argv);
#endif
	void ExecEventAsync(const char* evtName, const char* format, ArgList* args);
};

DWORD WINAPI ScriptThread(void* data);

void SpawnEvent(Event* evt);
