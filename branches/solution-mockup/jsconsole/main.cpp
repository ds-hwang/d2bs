#include <windows.h>
#include <shlwapi.h>
#include <io.h>
#include <errno.h>
#include <string>

#define __USE_SPIDERMONKEY__

#include "ScriptEngine.h"
#include "Script.h"

using namespace std;
using namespace botsys;

int main(int argc, char** argv)
{
	char path[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), path, MAX_PATH);
	PathRemoveFileSpec(path);

	ScriptEngine::Startup(path, 0x1000000);

	for(int i = 0; i < argc; i++)
	{
		// check if this file exists
		if(!(_access(argv[i], 0) != 0 && errno == ENOENT))
		{
			Script* script = ScriptEngine::FindOrCompile(string(argv[i]));
			if(script)
				script->Run();
		}
	}

	ScriptEngine::Shutdown();
	return 0;
}
