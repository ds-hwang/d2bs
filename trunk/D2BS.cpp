// Diablo II Botting System Core

#include <shlwapi.h>
#include <io.h>
#include <fcntl.h>

#include "dde.h"
#include "Offset.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Handlers.h"
#include "D2Ptrs.h"

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
				GetModuleFileName(hDll, Vars.szPath, MAX_PATH);
				PathRemoveFileSpec(Vars.szPath);
				strcat_s(Vars.szPath, MAX_PATH, "\\");
			}

#ifdef DEBUG
			char errlog[516] = "";
			sprintf_s(errlog, 516, "%sd2bs.log", Vars.szPath);
			AllocConsole();
			int handle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
			FILE* f = _fdopen(handle, "wt");
			*stderr = *f;
			setvbuf(stderr, NULL, _IONBF, 0);
			freopen_s(&f, errlog, "a+t", f);
#endif

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
	InitializeCriticalSection(&Vars.cGameLoopSection);

	Vars.bNeedShutdown = TRUE;
	Vars.bChangedAct = FALSE;
	Vars.bGameLoopEntered = FALSE;

	Vars.SectionCount = 0;

	DefineOffsets();
	InstallPatches();
	CreateDdeServer();

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

	SetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

	RemovePatches();
	ShutdownDdeServer();

	KillTimer(D2GFX_GetHwnd(), Vars.uTimer);

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
	DeleteCriticalSection(&Vars.cGameLoopSection);

	Log("D2BS Shutdown complete.");
	Vars.bNeedShutdown = false;
}
