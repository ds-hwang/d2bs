#pragma once

#ifndef __SCRIPTENGINE_H__
#define __SCRIPTENGINE_H__

#include <windows.h>
#include <io.h>
#include <errno.h>

#include <map>
#include <list>
#include <string>
#include <algorithm>

#include "Script.h"
#include "yasper.h"

namespace botsys
{

typedef std::map<std::string, ScriptPtr> ScriptMap;
typedef std::list<ScriptPtr> ScriptList;

class Engine;

typedef Engine* (__fastcall *CreateEngineFunc)(void);

enum EngineState
{
	NotRunning,
	Starting,
	Started,
	Stopping,
	Stopped
};

class Engine
{
private:
	CRITICAL_SECTION lock;
	EngineState state;
	ScriptMap scripts;
	std::string scriptPath;
	void* pData;

	// disable the default ctor, the copy ctor, and the assignment op
	Engine(const ScriptEngine&);
	Engine& operator=(const ScriptEngine&);

public:
	Engine();
	virtual ~Engine() = 0;

	static Engine* Create(std::string& engineName)
	{
		// cache engine procs to speed up execution
		// strictly speaking: this isn't necessary, but if for whatever reason, the implementer
		// decides to create a bunch of new Engines _A LOT_, this could speed up performance
		static std::map<std::string, CreateEngineFunc> loadedEngines = std::map<std::string, CreateEngineFunc>();

		// find the requested engine type
		if(loadedEngines.count(engineName) > 0)
		{
			return loadedEngines[engineName]();
		}

		std::string engineDllName = engineName + ".dll";
		if(_access(engineDllName.c_str(), 0) != 0 && errno == ENOENT)
			return NULL;

		// load the requested engine dll
		HMODULE engineDll = LoadLibraryEx(engineDllName.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		// does it have a CreateEngine function?
		CreateEngineFunc CreateEngine = (CreateEngineFunc)GetProcAddress(engineDll, "@CreateEngine@0");
		Engine* result = NULL;
		if(CreateEngine != NULL)
		{
			loadedEngines[engineName] = CreateEngine;
			result = CreateEngine();
		}

		FreeLibrary(engineDll);
		return result;
	}

	// Compile needs to ensure that it adds the script to the list
	virtual ScriptPtr Compile(std::string& filename, bool recompile = false) = 0;

	ScriptPtr Find(std::string& filename)
	{
		return scripts.count(filename) ? scripts[filename] : NULL;
	}
	ScriptPtr FindOrCompile(std::string& filename, bool recompile = false)
	{
		Script* result = Find(filename);
		if(!result)
			result = Compile(filename, recompile);
		return result;
	}
	void Release(ScriptPtr script)
	{
		script->End();

		// make sure we have exclusive access to the list while we fiddle with it
		EnterCriticalSection(&lock);
		scripts.erase(script->GetFilename());
		LeaveCriticalSection(&lock);
	}

	void GetScripts(ScriptList& list)
	{
		// clear the (possibly tainted) list
		list.clear();

		// make sure we have exclusive access to the list while we copy it
		EnterCriticalSection(&lock);

		for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
			list.push_back(it->second);

		LeaveCriticalSection(&lock);
	}

	// intentionally leaving this as a ptr
	void* GetPrivateData(void) { return pData; }
	void SetPrivateData(void* data) { EnterCriticalSection(&lock); pData = data; LeaveCriticalSection(&lock); }

	virtual void Startup(std::string& path, unsigned long maxBytes) = 0;
	virtual void Shutdown(void) = 0;
};

typedef yasper::ptr<Engine> EnginePtr;

}

#endif
