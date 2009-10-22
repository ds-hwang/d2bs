#pragma once

#include <windows.h>
#include <map>
#include <list>
#include <string>

#include "Script.h"

#if defined(__USE_SPIDERMONKEY__)
#pragma comment(lib, "spidermonkey.lib")
#elif defined(__USE_V8__)
#pragma comment(lib, "v8.lib")
#endif

#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

namespace botsys
{

typedef std::map<std::string, Script*> ScriptMap;

enum EngineState
{
	NotRunning,
	Starting,
	Started,
	Stopping,
	Stopped
};

class ScriptEngine
{
private:
	static CRITICAL_SECTION lock;
	static EngineState state;
	static ScriptMap scripts;

	// disable the default ctor, the copy ctor, and the assignment op
	ScriptEngine();
	~ScriptEngine();
	ScriptEngine(const ScriptEngine&);
	ScriptEngine& operator=(const ScriptEngine&);

public:
	static Script* Compile(std::string filename);
	static Script* Find(std::string filename)
	{
		return scripts.count(filename) ? scripts[filename] : NULL;
	}
	static Script* FindOrCompile(std::string filename)
	{
		Script* result = Find(filename);
		if(!result)
			result = Compile(filename);
		return result;
	}
	static void ReleaseScript(Script* script)
	{
		scripts.erase(script->GetFilename());
		delete script;
	}

	static void Startup(void);
	static void Shutdown(void);
};

}

#endif
