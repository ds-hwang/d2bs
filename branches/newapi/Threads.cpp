#include <windows.h>
#include <shlwapi.h>

#include "jsutilities.h"
#include "Script.h"

#include "helpers.h"
#include "Events.h"

#include "prthread.h"

#include "D2Utilities.h"
#include "Functions.h"
#include "Map.h"

#include "debug_new.h"

// Globals
extern HMODULE D2BSModule;
extern char D2BSModulePath[_MAX_PATH];

void MainThread(void* lpData)
{
	Log("MainThread Initialized");

	char path[_MAX_PATH + _MAX_FNAME], ini[_MAX_PATH + _MAX_FNAME], log[_MAX_PATH + _MAX_FNAME];

	sprintf(ini, "%sd2bs.ini", D2BSModulePath);
	sprintf(log, "%sd2bs.log", D2BSModulePath);

	freopen(log, "a+t", stderr);

	Config config;
	if(!LoadConfig(ini, &config))
	{
		Log("Loading configuration failed: couldn't find 'd2bs.ini'! Writing a new one...");
		WriteConfig(ini);
		LoadConfig(ini, &config);
	}

	sprintf(path, "%s\\%s", D2BSModulePath, config.scriptPath);

	Script::Startup(path, InitContext, InitScript);
	Script::SetBranchCallback(branch);

	Script* DefaultScript = Script::CompileFile("default.dbj", false);
	if(DefaultScript)
		DefaultScript->Start();

	while(Script::IsActive())
	{
		PR_Sleep(1000);
		// TODO: Process stuff here, like events.
		// this processes at 50 just fine, but has an upper bound of ~18 events/sec
		PulseEvent();
		if(GetPlayerUnit())
		{
			PR_Sleep(5000);
			Map map(1, true);
			map.Dump();
		}
	}

	Log("MainThread Terminated");
}
