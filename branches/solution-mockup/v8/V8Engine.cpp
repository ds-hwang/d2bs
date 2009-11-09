#include "Engine.h"
#include "V8Engine.h"

namespace botsys
{

ScriptPtr Engine::Compile(std::string& filename, bool recompile)
{
	// TODO: build the Script object, compile the file, then assign the
	// result of that to the private data of the Script and add the new
	// Script to the list
	return NULL;
}

void Engine::Startup(std::string& path, unsigned long maxBytes)
{
}

void Engine::Shutdown(void)
{
}

}
