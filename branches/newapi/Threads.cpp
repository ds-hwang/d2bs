#include <windows.h>
#include <shlwapi.h>

#include "jsutilities.h"
#include "helpers.h"
#include "Script.h"
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

	Script* OOGScript = Script::CompileFile("starter.dbj", false);
	Script* GameScript = Script::CompileFile("default.dbj", false);
	if(OOGScript)
		OOGScript->Start();

	while(Script::IsActive())
	{
		// TODO: Process stuff here, like events.
		// this processes at 50 just fine, but has an upper bound of ~75 events/sec
		PR_Sleep(1000);
		PulseEvent();
		GamePrint("D2BS:: Initialized");
		if (GameReady() && GameScript && !GameScript->IsRunning())
		{
			GameScript->Start();
		}
		else if (!GameReady())
		{
			GameScript->Stop();
			GameScript->Abort();
		}
	}
}
