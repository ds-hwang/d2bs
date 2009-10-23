#include "ScriptEngine.h"

namespace botsys
{

CRITICAL_SECTION ScriptEngine::lock = {0};
EngineState ScriptEngine::state = NotRunning;
ScriptMap ScriptEngine::scripts = ScriptMap();

ScriptPtr ScriptEngine::Compile(std::string filename, bool recompile)
{
	// TODO: build the Script object, compile the file, then assign the
	// result of that to the private data of the Script and add the new
	// Script to the list
	return NULL;
}

void ScriptEngine::Startup(std::string path, unsigned long maxBytes)
{
}

void ScriptEngine::Shutdown(void)
{
}

}
