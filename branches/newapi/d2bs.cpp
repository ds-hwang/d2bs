#include <windows.h>
#include <shlwapi.h>

#include "helpers.h"
#include "jsutilities.h"
#include "Script.h"
#include "Threads.h"

#include "nspr/prthread.h"
#include "nspr/prinit.h"

#include "d2bs.h"

#include "debugnew/debug_new.h"


BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hDll);
	static PRThread* thread = NULL;
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		char path[_MAX_PATH], ini[_MAX_PATH + _MAX_FNAME], log[_MAX_PATH + _MAX_FNAME];
		GetModuleFileName(hDll, path, _MAX_FNAME + _MAX_PATH);
		PathRemoveFileSpec(path);
		sprintf(ini, "%s\\d2bs.ini", path);
		sprintf(log, "%s\\d2bs.log", path);

		freopen(log, "a+t", stderr);

		Config config;
		if(!LoadConfig(ini, &config))
		{
			Log("Loading configuration failed: couldn't find 'd2bs.ini'!");
			return FALSE;
		}

		sprintf(path, "%s\\%s", path, config.scriptPath);
		Script::Startup(path, InitContext, InitScript);
		Script::SetBranchCallback(branch);
		thread = PR_CreateThread(PR_USER_THREAD, MainThread, 0, PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD, PR_JOINABLE_THREAD, 0);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		Script::Shutdown();
		PR_Cleanup();
	}
	return TRUE;
}
