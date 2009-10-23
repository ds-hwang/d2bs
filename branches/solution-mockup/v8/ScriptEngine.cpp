#include "ScriptEngine.h"

namespace botsys
{

CRITICAL_SECTION ScriptEngine::lock = {0};
EngineState ScriptEngine::state = NotRunning;
ScriptMap ScriptEngine::scripts = ScriptMap();

yasper::ptr<Script> ScriptEngine::Compile(std::string filename, bool recompile)
{
	return NULL;
}

void ScriptEngine::Startup(std::string path, unsigned long maxBytes)
{
}

void ScriptEngine::Shutdown(void)
{
}

}
