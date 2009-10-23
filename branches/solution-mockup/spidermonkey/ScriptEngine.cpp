#include "ScriptEngine.h"

#define XP_WIN

#include "jsapi.h"

namespace botsys
{

CRITICAL_SECTION ScriptEngine::lock = {0};
EngineState ScriptEngine::state = NotRunning;
ScriptMap ScriptEngine::scripts = ScriptMap();
std::string ScriptEngine::scriptPath = std::string("");
void* ScriptEngine::pData = NULL;

yasper::ptr<Script> ScriptEngine::Compile(std::string filename, bool recompile)
{
	if(recompile && scripts.count(filename))
		Release(scripts[filename]);
	// TODO: build the Script object, run JS_CompileFile on the filename,
	// then assign the result of that to the private data of the Script
	return NULL;
}

void ScriptEngine::Startup(std::string path, unsigned long maxBytes)
{
	if(state == NotRunning)
	{
		InitializeCriticalSection(&lock);

		EnterCriticalSection(&lock);
		state = Starting;
		scriptPath = path;
		pData = (void*)JS_NewRuntime(maxBytes);
		LeaveCriticalSection(&lock);
	}
}

void ScriptEngine::Shutdown(void)
{
	if(state == Started)
	{
		EnterCriticalSection(&lock);
		for(ScriptMap::iterator it = scripts.begin(); it != scripts.end(); it++)
		{
			it->second->End();
		}
		JS_DestroyRuntime((JSRuntime*)pData);
		JS_ShutDown();
		LeaveCriticalSection(&lock);

		DeleteCriticalSection(&lock);
	}
}

}
