#include "Engine.h"
#include "SMEngine.h"

namespace botsys
{

ScriptPtr Engine::Compile(std::string& filename, bool recompile)
{
	if(recompile && scripts.count(filename))
		Release(scripts[filename]);
	// TODO: build the Script object, run JS_CompileFile on the filename,
	// then assign the result of that to the private data of the Script
	// and add the new Script to the list
	return NULL;
}

void Engine::Startup(std::string& path, unsigned long maxBytes)
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

void Engine::Shutdown(void)
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
