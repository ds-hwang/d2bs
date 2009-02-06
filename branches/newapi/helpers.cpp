#include "helpers.h"
#include <io.h>
#include <windows.h>
#include <cstring>

bool LoadConfig(const char* file, Config* config)
{
	if(_access(file, 0) != 0)
		return false;

	GetPrivateProfileString("setting", "ScriptPath", "scripts", config->scriptPath, _MAX_FNAME, file);

	return true;
}
