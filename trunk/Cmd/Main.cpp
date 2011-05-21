#define EXPORTED
#include "ScriptEngine.hpp"

#include <io.h>
#include <direct.h>

int main(const char** argc, int argv)
{
	if(argv < 1)
	{
		printf("You must specify a file name!");
		return 0;
	}

	char path[MAX_PATH], script[MAX_PATH];
	_getcwd(path, MAX_PATH);
	sprintf_s(script, MAX_PATH, "%s\\%s", path, argc[0]);

	if(_access(script, 0) != -1)
	{
		ScriptEngine engine(path, 0x2800);
		Script* script = engine.Compile(argc[0]);
		script->Start();
	}
}
