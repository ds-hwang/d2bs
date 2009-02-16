#include <windows.h>
#include <shlwapi.h>

#include "jsutilities.h"
#include "Script.h"

#include "helpers.h"
#include "Events.h"

#include "prthread.h"

#include "D2Utilities.h"

#include "debug_new.h"

extern HMODULE D2BSModule;

void MainThread(void* lpData)
{
	char path[_MAX_PATH], ini[_MAX_PATH + _MAX_FNAME], log[_MAX_PATH + _MAX_FNAME];
	GetModuleFileName(D2BSModule, path, _MAX_FNAME + _MAX_PATH);
	PathRemoveFileSpec(path);
	sprintf(ini, "%s\\d2bs.ini", path);
	sprintf(log, "%s\\d2bs.log", path);

	freopen(log, "a+t", stderr);

	Config config;
	if(!LoadConfig(ini, &config))
	{
		Log("Loading configuration failed: couldn't find 'd2bs.ini'! Writing a new one...");
		WriteConfig(ini);
		LoadConfig(ini, &config);
	}

	sprintf(path, "%s\\%s", path, config.scriptPath);

	Script::Startup(path, InitContext, InitScript);
	Script::SetBranchCallback(branch);

	Script* DefaultScript = Script::CompileFile("default.dbj", false);
	if(DefaultScript)
		DefaultScript->Start();

	while(Script::IsActive())
	{
		PR_Sleep(50);
		// TODO: Process stuff here, like events.
		// this processes at 50 just fine, but has an upper bound of ~18 events/sec
		PulseEvent();
	}
}
