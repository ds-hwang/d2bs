#include "ScriptEngine.h"

namespace botsys
{

CRITICAL_SECTION ScriptEngine::lock = {0};
EngineState ScriptEngine::state = NotRunning;
ScriptMap ScriptEngine::scripts = ScriptMap();

Script* ScriptEngine::Compile(std::string filename)
{
	return NULL;
}

void ScriptEngine::Startup(void)
{
}

void ScriptEngine::Shutdown(void)
{
}

}
