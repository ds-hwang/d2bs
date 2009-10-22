#include <windows.h>
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
	ScriptEngine::Startup();

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
