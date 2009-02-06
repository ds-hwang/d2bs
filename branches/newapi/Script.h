#pragma once

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <windows.h>
#include <cstdlib>
#include <map>
#include <list>
#include <string>
#include "js32.h"
#include "nspr/prthread.h"

#include "debugnew/debug_new.h"

enum ScriptType { File, Command };

static JSClass global_obj = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

class AutoRoot;
class Script;

// standard typedefs for convenience
typedef std::map<std::string, bool> IncludeList;

typedef std::list<AutoRoot*> EventList;
typedef EventList::iterator EventIterator;
typedef std::map<std::string, EventList> EventMap;
typedef std::list<PRThread*> ActiveEventList;

typedef std::list<Script*> ScriptList;
typedef ScriptList::iterator ScriptIterator;
typedef std::map<std::string, Script*> ScriptMap;

typedef void (__fastcall *ScriptCallback)(Script*);

class Script
{
protected:
	static ScriptMap scripts;
	static JSRuntime* runtime;
	static CRITICAL_SECTION section;
	static char scriptPath[_MAX_FNAME + _MAX_PATH];

	static ScriptCallback scriptCallback;

	ScriptType type;
	std::string file;
	IncludeList includes;
	EventMap eventFunctions;
	ActiveEventList activeEvents;

	CRITICAL_SECTION scriptSection;

	JSObject *scriptObject, *globalObject;
	JSContext *context;
	JSScript *script;

	int runCount;
	bool isPaused, isAborted;

	PRThread* thread;

	static void LockAll(void);
	static void UnlockAll(void);

	Script(ScriptType, const char*);
	~Script(void);

	// prevent copy and assignment
	Script(const Script&);
	Script& operator=(const Script&);

public:
	static const char* GetScriptPath(void);
	static Script* CompileFile(const char* file, bool recompile);
	static Script* CompileCommand(const char* command, bool recompile);

	static void Startup(const char* basePath, JSContextCallback contextCallback, ScriptCallback scriptCallback = NULL);
	static void Shutdown(void);
	static bool IsActive(void);

	static ScriptList GetAllScripts(void);

	void Start(void);
	void Run(void);
	void Pause(void);
	void Resume(void);
	void Stop(void);

	bool IsRunning(void);
	bool IsPaused(void);
	bool IsAborted(void);

	void Lock();
	void Unlock();

	JSContext* GetContext(void) { return context; }
	JSObject* GetGlobalObject(void) { return globalObject; }
};

void ScriptThread(LPVOID lpData);
void EventThread(LPVOID lpData);

#endif
