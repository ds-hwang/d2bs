#include <windows.h>
#include <shlwapi.h>

#include "helpers.h"
#include "jsutilities.h"
#include "Script.h"
#include "Threads.h"

#include "nspr/prthread.h"
#include "nspr/prinit.h"

#include "D2BS.h"

#include "debugnew/debug_new.h"


BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hDll);
	static PRThread* D2BSThread = NULL;

	static OSVERSIONINFOEX os = {0};
	if(os.dwMajorVersion == 0)
	{
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&os);
	}

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
			// TODO: Add code to write a default ini file and load that instead of detaching
			return FALSE;
		}

		sprintf(path, "%s\\%s", path, config.scriptPath);
		Script::Startup(path, InitContext, InitScript);
		Script::SetBranchCallback(branch);
		D2BSThread = PR_CreateThread(PR_USER_THREAD, MainThread, 0, PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD, PR_JOINABLE_THREAD, 0);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		// TODO: Test to see if threads don't blow up on OS above Windows XP.
		Script::Shutdown();
		if(os.dwMajorVersion > 5)
		{
			if(PR_Initialized() == PR_TRUE)
				PR_Cleanup();
		}
		// BOOM -- UNGRACEFUL SHUTDOWN!!
	}
	return TRUE;
}
