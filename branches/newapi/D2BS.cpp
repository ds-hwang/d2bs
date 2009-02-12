#include <windows.h>
#include <shlwapi.h>

#include "helpers.h"
#include "jsutilities.h"
#include "Script.h"
#include "Threads.h"

#include "prthread.h"
#include "prinit.h"

#include "D2BS.h"

#include "debug_new.h"

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MSVC_DEBUG
	new_verbose_flag = false;
#endif
	DisableThreadLibraryCalls(hDll);
	static PRThread* D2BSThread = NULL;

	static HANDLE EventHandle = NULL;
	if(!EventHandle)
		EventHandle = CreateEvent(0, TRUE, FALSE, "D2BS\\ShutdownEvent");

	/*static OSVERSIONINFOEX os = {0};
	if(os.dwMajorVersion == 0)
	{
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx((LPOSVERSIONINFO)&os);
	}*/

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
			Log("Loading configuration failed: couldn't find 'd2bs.ini'! Writing a new one...");
			WriteConfig(ini);
			LoadConfig(ini, &config);
		}

		sprintf(path, "%s\\%s", path, config.scriptPath);
		Script::Startup(path, InitContext, InitScript);
		Script::SetBranchCallback(branch);
		D2BSThread = PR_CreateThread(PR_USER_THREAD, MainThread, 0, PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD, PR_JOINABLE_THREAD, 0);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		/* This section of the code needs to be refactored in due time. 
		On NT Operating Systems, all threads are terminated before DLL_PROCESS_DETACH.
		Eventually we need to figure out the best way to hook Diablo II's closing
		procedure, and call a method to shut our threads down safely before FreeLibrary
		is called. */

		Script::Shutdown();

		WaitForSingleObject(EventHandle, INFINITE);
		CloseHandle(EventHandle);
		// On Operating Systems, greater than, XP.
		/*if(os.dwMajorVersion > 5)
		{
			if(D2BSThread)
				PR_JoinThread(D2BSThread);
		}*/
	}
	return TRUE;
}
