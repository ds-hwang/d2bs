#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, void* reserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hDll);
	}
	return TRUE;
}
