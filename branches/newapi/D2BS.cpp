#include <windows.h>

#include "D2BS.h"
#include "Offsets.h"

#include "debug_new.h"

HMODULE D2BSModule = NULL;

BOOL WINAPI DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MSVC_DEBUG
	new_verbose_flag = false;
#endif

	if(dwReason == DLL_PROCESS_ATTACH)
	{
		ApplyPatches();
		D2BSModule = hDll;
		DisableThreadLibraryCalls(hDll);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		RemovePatches();
	}
	return TRUE;
}
