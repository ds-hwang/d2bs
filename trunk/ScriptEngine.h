#pragma once
#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

#include "js32.h"
#include "Script.h"

#include <list>
#include <map>
#include <string>

typedef std::map<std::string, Script*> ScriptMap;

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

	static JSRuntime* runtime;
	static ScriptMap scripts;
	static EngineState state;
	static CRITICAL_SECTION lock;

public:
	friend class Script;

	static BOOL Startup(void);
	static void Shutdown(void);
	static EngineState GetState(void) { return state; }

	static void FlushCache(void);

	static Script* CompileFile(const char* file, ScriptState state, bool recompile = false);
	static Script* CompileCommand(const char* command);
	static void DisposeScript(Script* script);

	static void GetScripts(ScriptList& list);
	static unsigned int GetCount(bool active = true, bool unexecuted = false);

	static JSRuntime* GetRuntime(void) { return runtime; }

	static void StopAll(bool forceStop = false);
	static void PauseAll(void);
	static void ResumeAll(void);
#if 0
// Disabled until decision is made to deorbit.
	static void ExecEvent(char* evtName, AutoRoot** argv, uintN argc);
#endif
	static void ExecEventAsync(char* evtName, AutoRoot** argv, uintN argc);
	static void InitClass(JSContext* context, JSObject* globalObject, JSClass* classp,
							 JSNative ctor, JSFunctionSpec* methods, JSPropertySpec* props,
							 JSFunctionSpec* s_methods, JSPropertySpec* s_props);
	static void DefineConstant(JSContext* context, JSObject* globalObject, const char* name, int value);
};

JSBool watchHandler(JSContext* cx, JSObject* obj, jsval id, jsval old, jsval* newval, void* closure);
JSTrapStatus debuggerCallback(JSContext* cx, JSScript* script, jsbytecode* pc, jsval* rval, void* closure);
JSTrapStatus exceptionCallback(JSContext* cx, JSScript* script, jsbytecode* pc, jsval* rval, void* closure);
void* executeCallback(JSContext* cx, JSStackFrame* frame, JSBool before, JSBool* ok, void* closure);
JSBool branchCallback(JSContext* cx, JSScript* script);
JSBool eventBranchCallback(JSContext* cx, JSScript* script);
JSBool contextCallback(JSContext* cx, uintN contextOp);
JSBool gcCallback(JSContext* cx, JSGCStatus status);
void reportError(JSContext *cx, const char *message, JSErrorReport *report);

#endif

