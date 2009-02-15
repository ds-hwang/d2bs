#include <windows.h>

#include "D2BS.h"
#include "Offsets.h"

#include "SyncLock.h"

#include "debug_new.h"

// TODO: Move these globals somewhere else
HMODULE D2BSModule = NULL;
HHOOK hKeybHook, hMouseHook;
bool bClickAction = false, bBlockMinimize = false;
WNDPROC oldWndProc = NULL;
DWORD dwSelectedUnitId = 0, dwSelectedUnitType = 0;

CRITICAL_SECTION PrintLock::SyncObj = {0};

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MSVC_DEBUG
	new_verbose_flag = false;
#endif

	if(dwReason == DLL_PROCESS_ATTACH)
	{
		PrintLock::Initialize();
		ApplyPatches();
		D2BSModule = hDll;
		DisableThreadLibraryCalls(hDll);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		RemovePatches();
		PrintLock::Destroy();
	}
	return TRUE;
}
