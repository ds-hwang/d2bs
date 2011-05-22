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

	wchar_t path[MAX_PATH], script[MAX_PATH];
	_wgetcwd(path, MAX_PATH);
	swprintf_s(script, MAX_PATH, L"%s\\%s", path, argc[0]);

	if(_waccess(script, 0) != -1)
	{
		ScriptEngine engine(path, 0x2800);
		std::string path(argc[0]);
		std::wstring wpath;
		wpath.assign(path.begin(), path.end());
		Script* script = engine.Compile(wpath.c_str());
		script->Start();
	}
}
