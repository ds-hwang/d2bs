#include <windows.h>
#include "d2bs.h"
#include "Script.h"
#include "Threads.h"
#include "nspr/prthread.h"

#include "debugnew/debug_new.h"

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
	static PRThread* thread = NULL;
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		Script::Startup();
		thread = PR_CreateThread(PR_USER_THREAD, MainThread, 0, PR_PRIORITY_NORMAL, PR_LOCAL_THREAD, PR_JOINABLE_THREAD, 0);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		Script::Shutdown();
		PR_JoinThread(thread);
	}
	return TRUE;
}