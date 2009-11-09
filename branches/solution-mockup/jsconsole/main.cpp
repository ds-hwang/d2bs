#include <windows.h>
#include <shlwapi.h>
#include <io.h>
#include <errno.h>
#include <string>

#define __USE_SPIDERMONKEY__

#include "Engine.h"
#include "Script.h"

using namespace std;
using namespace botsys;

int main(int argc, char** argv)
{
	char path[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);
	PathRemoveFileSpec(path);
	string engineName = string("spidermonkey"), scriptPath = string(path);

	Engine* engine = Engine::Create(engineName);
	engine->Startup(scriptPath, 0x1000000);

	for(int i = 0; i < argc; i++)
	{
		// check if this file exists
		if(!(_access(argv[i], 0) != 0 && errno == ENOENT))
		{
			string scriptName = string(argv[i]);
			ScriptPtr script = engine->FindOrCompile(scriptName);
			if(script)
				script->Run();
		}
	}

	engine->Shutdown();
	delete engine;
	return 0;
}
