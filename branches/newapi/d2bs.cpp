#include <windows.h>
#include "helpers.h"
#include "jsutilities.h"
#include "d2bs.h"
#include "Script.h"
#include "Threads.h"
#include "nspr/prthread.h"
#include <shlwapi.h>

#include "debugnew/debug_new.h"

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hDll);
	static PRThread* thread = NULL;
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		char path[_MAX_PATH], d2bsini[_MAX_PATH + _MAX_FNAME];
		GetModuleFileName(hDll, path, _MAX_FNAME + _MAX_PATH);
		PathRemoveFileSpec(path);
		sprintf(d2bsini, "%s\\d2bs.ini", path);

		Config config;
		if(!LoadConfig(d2bsini, &config))
			return FALSE;

		sprintf(path, "%s\\%s", path, config.scriptPath);
		Script::Startup(path, InitContext);
		thread = PR_CreateThread(PR_USER_THREAD, MainThread, 0, PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD, PR_JOINABLE_THREAD, 0);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		Script::Shutdown();
		PR_JoinThread(thread);
	}
	return TRUE;
}