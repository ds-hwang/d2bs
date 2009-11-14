// Diablo II Botting System Core

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "dde.h"
#include "Offset.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Handlers.h"
#include "Console.h"
#include "D2BS.h"


#ifdef _MSVC_DEBUG
#include "D2Loader.h"
#endif

static HANDLE hD2Thread = INVALID_HANDLE_VALUE;

BOOL WINAPI DllMain(HINSTANCE hDll,DWORD dwReason,LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hDll);
			if(lpReserved != NULL)
			{
				Vars.pModule = (Module*)lpReserved;

				if(!Vars.pModule)
					return FALSE;

				strcpy_s(Vars.szPath, MAX_PATH, Vars.pModule->szPath);
			}
			else
			{
				GetModuleFileName(hDll,Vars.szPath,MAX_PATH);
				PathRemoveFileSpec(Vars.szPath);
				strcat_s(Vars.szPath, MAX_PATH, "\\");
			}
			Vars.bShutdownFromDllMain = FALSE;
			if(!Startup())
				return FALSE;
		}
		break;
		case DLL_PROCESS_DETACH:
			if(Vars.bNeedShutdown)
			{
				Vars.bShutdownFromDllMain = TRUE;
				Shutdown();
			}
		break;
	}

	return TRUE;
}

BOOL Startup(void)
{
	InitializeCriticalSection(&Vars.cRoomSection);
	InitializeCriticalSection(&Vars.cMiscSection);
	InitializeCriticalSection(&Vars.cScreenhookSection);
	InitializeCriticalSection(&Vars.cPrintSection);
	InitializeCriticalSection(&Vars.cBoxHookSection);
	InitializeCriticalSection(&Vars.cFrameHookSection);
	InitializeCriticalSection(&Vars.cLineHookSection);
	InitializeCriticalSection(&Vars.cImageHookSection);
	InitializeCriticalSection(&Vars.cTextHookSection);
	InitializeCriticalSection(&Vars.cFlushCacheSection);
	InitializeCriticalSection(&Vars.cConsoleSection);

	Genhook::Initialize();
	DefineOffsets();
	InstallPatches();
	CreateDdeServer();

	Vars.bNeedShutdown = TRUE;
	if((hD2Thread = CreateThread(NULL, NULL, D2Thread, NULL, NULL, NULL)) == INVALID_HANDLE_VALUE)
		return FALSE;

	return TRUE;
}

void Shutdown(void)
{
	if(!Vars.bNeedShutdown)
		return;

	Vars.bActive = FALSE;
	if(!Vars.bShutdownFromDllMain)
		WaitForSingleObject(hD2Thread, INFINITE);

	SetWindowLong(D2WIN_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

	delete Vars.image;
	delete Vars.text;

	RemovePatches();
	Console::Destroy();
	Genhook::Destroy();
	ShutdownDdeServer();

	UnhookWindowsHookEx(Vars.hMouseHook);
	UnhookWindowsHookEx(Vars.hKeybHook);

	DeleteCriticalSection(&Vars.cRoomSection);
	DeleteCriticalSection(&Vars.cMiscSection);
	DeleteCriticalSection(&Vars.cScreenhookSection);
	DeleteCriticalSection(&Vars.cPrintSection);
	DeleteCriticalSection(&Vars.cBoxHookSection);
	DeleteCriticalSection(&Vars.cFrameHookSection);
	DeleteCriticalSection(&Vars.cLineHookSection);
	DeleteCriticalSection(&Vars.cImageHookSection);
	DeleteCriticalSection(&Vars.cTextHookSection);
	DeleteCriticalSection(&Vars.cFlushCacheSection);
	DeleteCriticalSection(&Vars.cConsoleSection);

	Log("D2BS Shutdown complete.");
	Vars.bNeedShutdown = false;
}
