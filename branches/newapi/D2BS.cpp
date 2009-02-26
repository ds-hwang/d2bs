#include <windows.h>

#include "prthread.h"

#include "Offsets.h"

#include "SyncLock.h"

#include "debug_new.h"
#include "helpers.h"
#include "D2BS.h"

// TODO: Move these globals somewhere else
HMODULE D2BSModule = NULL;
char D2BSModulePath[_MAX_PATH];

HHOOK hKeybHook, hMouseHook;
bool bClickAction = false, bBlockMinimize = false;
WNDPROC oldWndProc = NULL;
DWORD dwSelectedUnitId = 0, dwSelectedUnitType = 0;
PRThread* mainThread = NULL;

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
		#ifndef _MSVC_DEBUG
			D2BSModule = ((Module*)lpReserved)->hModule;
			if (!D2BSModule)
				return false;
			strcpy_s(D2BSModulePath, MAX_PATH, ((Module*)lpReserved)->szPath);
		#else
			D2BSModule = hDll;
			GetModuleFileName(D2BSModule, D2BSModulePath, _MAX_FNAME + _MAX_PATH);
			PathRemoveFileSpec(D2BSModulePath);
			strcat(D2BSModulePath,"\\");
		#endif

		DisableThreadLibraryCalls(hDll);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		RemovePatches();
		PrintLock::Destroy();
	}
	return TRUE;
}
