#pragma once
#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

#include <list>
#include <map>
#include <string>
#include <windows.h>

#include "js32.h"
#include "Script.h"

typedef std::map<std::string, Script*> ScriptMap;

typedef bool (__fastcall *ScriptCallback)(Script*, void*, uint);

struct EventHelper;

enum EngineState {
	Starting,
	Running,
	Paused,
	Stopping,
	Stopped
};

class ScriptEngine
{
	ScriptEngine(void) {};
	virtual ~ScriptEngine(void) = 0;
	ScriptEngine(const ScriptEngine&);
	ScriptEngine& operator=(const ScriptEngine&);
	friend void __cdecl EventThread(void* arg);

	static JSRuntime* runtime;
	static ScriptMap scripts;
	static EngineState state;
	static CRITICAL_SECTION lock;
	static SLIST_HEADER eventList;
	static HANDLE eventHandle;
	static JSContext* context;

public:
	friend class Script;

	static BOOL Startup(void);
	static void Shutdown(void);
	static EngineState GetState(void) { return state; }

	static void FlushCache(void);

	static Script* CompileFile(const char* file, ScriptState state, bool recompile = false);
	static Script* CompileCommand(const char* command);
	static void DisposeScript(Script* script);

	static void ForEachScript(ScriptCallback callback, void* argv, uint argc);
	static unsigned int GetCount(bool active = true, bool unexecuted = false);

	static JSRuntime* GetRuntime(void) { return runtime; }

	static void StopAll(bool forceStop = false);
	static void ExecEventAsync(char* evtName, char* format, uintN argc, ...);
	static void PushEvent(EventHelper* helper);

	static void InitClass(JSContext* context, JSObject* globalObject, JSClass* classp,
							 JSFunctionSpec* methods, JSPropertySpec* props,
							 JSFunctionSpec* s_methods, JSPropertySpec* s_props);
	static void DefineConstant(JSContext* context, JSObject* globalObject, const char* name, int value);

	friend JSBool gcCallback(JSContext* cx, JSGCStatus status);
};

// these ForEachScript helpers are exposed in case they can be of use somewhere
bool __fastcall StopIngameScript(Script* script, void*, uint);
bool __fastcall ExecEventOnScript(Script* script, void* argv, uint argc);
struct EventHelper
{
	PSLIST_ENTRY Next;
	char evtName[15];
	char format[10];
	bool executed;
	uintN argc;
	uintN argvsize;
	BYTE* argv;
};

JSBool branchCallback(JSContext* cx);
JSBool contextCallback(JSContext* cx, uintN contextOp);
JSBool gcCallback(JSContext* cx, JSGCStatus status);
void reportError(JSContext *cx, const char *message, JSErrorReport *report);

#endif
