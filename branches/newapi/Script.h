#pragma once

#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#include <windows.h>
#include <cstdlib>
#include <map>
#include <list>
#include <string>
#include "js32.h"

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
typedef std::list<HANDLE> ActiveEventList;

typedef std::list<Script*> ScriptList;
typedef ScriptList::iterator ScriptIterator;
typedef std::map<std::string, Script*> ScriptMap;

class Script
{
protected:
	static ScriptMap scripts;
	static JSRuntime* runtime;
	static CRITICAL_SECTION section;

	ScriptType type;
	std::string file;
	IncludeList includes;
	EventMap eventFunctions;
	ActiveEventList activeEvents;

	CRITICAL_SECTION scriptSection;

	JSObject *scriptObject, *globalObject, *meObject;
	JSContext *context;
	JSScript *script;

	int runCount;

private:
	Script(ScriptType, const char*);
	~Script(void);

	// prevent copy and assignment
	Script(const Script&);
	Script& operator=(const Script&);

public:
	static Script* CompileFile(const char* file, bool recompile);
	static Script* CompileCommand(const char* command, bool recompile);

	static void Startup(void);
	static void Shutdown(void);
	static bool IsActive(void);

	static void LockAll(void);
	static void UnlockAll(void);

	static ScriptList GetAllScripts(void);
};

#endif
