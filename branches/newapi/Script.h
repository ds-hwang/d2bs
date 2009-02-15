#pragma once

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <windows.h>
#include <cstdlib>
#include <map>
#include <list>
#include <string>

#include "js32.h"
#include "prthread.h"

enum ScriptType { File, Command };

static JSClass global_obj = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

class Script;
class AutoRoot;

// standard typedefs for convenience
typedef std::map<std::string, bool> IncludeList;

typedef std::list<AutoRoot*> ArgList;
typedef std::list<AutoRoot*> EventList;
typedef EventList::iterator EventIterator;
typedef std::map<std::string, EventList> EventMap;
typedef std::list<PRThread*> ActiveEventList;

typedef std::list<Script*> ScriptList;
typedef ScriptList::iterator ScriptIterator;
typedef std::map<std::string, Script*> ScriptMap;

typedef std::list<std::string> IncludePaths;
typedef IncludePaths::iterator IncludePathIterator;

typedef void (__fastcall *ScriptCallback)(Script*, bool);

// supporting objects
struct EventData
{
	Script* owner;
	JSObject* globalObject;
	uintN argc;
	ArgList argv;
	EventList eventFuncs;
};

class AutoRoot
{
private:
	JSContext* cx;
	jsval root;
public:
	AutoRoot(JSContext* ncx, jsval val) : cx(ncx), root(val) { if(cx) JS_AddRoot(cx, &root); }
	~AutoRoot() { if(cx) JS_RemoveRoot(cx, &root); cx = NULL; }
	jsval value() { return root; }
};

class Script
{
protected:
	static ScriptMap scripts;
	static JSRuntime* runtime;
	static CRITICAL_SECTION section;
	static char scriptPath[_MAX_FNAME + _MAX_PATH];

	static ScriptCallback scriptCallback;
	static JSBranchCallback branchCallback;

	ScriptType type;
	std::string file;
	IncludePaths includeDirs;
	IncludeList includes;
	EventMap eventFunctions;
	ActiveEventList activeEvents;

	// TODO: Use PRLock instead
	CRITICAL_SECTION scriptSection;

	JSObject *scriptObject, *globalObject;
	JSContext *context;
	JSScript *script;

	int runCount;
	bool isPaused, isAborted;

	PRThread* scriptThread;

	static void LockAll(void) { EnterCriticalSection(&section); }
	static void UnlockAll(void) { LeaveCriticalSection(&section); }

	Script(ScriptType, const char*);
	~Script(void);

	// prevent copy and assignment
	Script(const Script&);
	Script& operator=(const Script&);

public:
	static const char* GetScriptPath(void);
	static Script* CompileFile(const char* file, bool recompile = false);
	static Script* CompileCommand(const char* command, bool recompile = false);
	static void FlushCache(void);
	inline static JSRuntime* GetRuntime(void) { return runtime; }

	static void Startup(const char* basePath, JSContextCallback contextCallback = NULL, ScriptCallback scriptCallback = NULL);
	static void Shutdown(void);
	static bool IsActive(void);

	static inline void SetBranchCallback(JSBranchCallback callback) { branchCallback = callback; }
	static inline JSBranchCallback GetBranchCallback(void) { return branchCallback; }

	static void AbortAll(void);
	static void PauseAll(void);
	static void ResumeAll(void);

	static ScriptList GetAllScripts(void);

	void Start(void);
	void Run(void);
	void Pause(void);
	void Resume(void);
	void Stop(void);
	void Abort(void);

	bool IsRunning(void);
	bool IsPaused(void);
	bool IsAborted(void);

	IncludePaths GetIncludePaths(void);
	void AddIncludePath(const char* dir);
	bool IsIncluded(const char* file);
	bool Include(const char* file);

	void AddEvent(const char* evtName, jsval evtFunc);
	void RemoveEvent(const char* evtName, jsval evtFunc);
	void ClearEvent(const char* evtName);
	void ClearAllEvents(void);
	void ExecEvent(const char* evtName, uintN argc, jsval* argv);

	void Lock() { EnterCriticalSection(&scriptSection); }
	void Unlock() { LeaveCriticalSection(&scriptSection); }

	inline JSContext* GetContext(void) { return context; }
	inline JSObject* GetGlobalObject(void) { return globalObject; }
	inline const char* GetFileName(void) { return file.c_str() + ((strlen(GetScriptPath())) + 1); }
	inline PRThread* GetThread(void) { return scriptThread; }
	inline PRThreadPriority GetThreadPriority(void) { return PR_GetThreadPriority(scriptThread); }
	inline void SetThreadPriority(PRThreadPriority priority) { return PR_SetThreadPriority(scriptThread, priority); }
	inline ActiveEventList& GetEventThreads(void) { return activeEvents; }
};

void ScriptThread(void* lpData);
void EventThread(void* lpData);

JSBool branchBack(JSContext* cx, JSScript* scr);

#endif
