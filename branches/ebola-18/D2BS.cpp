// Diablo II Botting System Core

#define RELDEBUG
#ifdef RELDEBUG
#define _MSVC_DEBUG
#define DEBUG
#pragma comment(lib, "shlwapi")
#endif

#ifdef _MSVC_DEBUG
#include <shlwapi.h>
#include "D2Loader.h"
#endif

#include "D2BS.h"
#include "dde.h"
#include "Offset.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Handlers.h"
#include "Console.h"

static HANDLE hD2Thread = INVALID_HANDLE_VALUE;

BOOL WINAPI DllMain(HINSTANCE hDll,DWORD dwReason,LPVOID lpReserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hDll);
#ifndef _MSVC_DEBUG
			Vars.pModule = (Module*)lpReserved;

			if(!Vars.pModule)
				return FALSE;

			strcpy_s(Vars.szPath, MAX_PATH, Vars.pModule->szPath);
#else 	
			GetModuleFileName(hDll,Vars.szPath,MAX_PATH);
			PathRemoveFileSpec(Vars.szPath);
			strcat_s(Vars.szPath, MAX_PATH, "\\");
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
	if((hD2Thread = CreateThread(NULL, NULL, D2Thread, NULL, CREATE_SUSPENDED, NULL)) == INVALID_HANDLE_VALUE)
		return FALSE;

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

	DefineOffsets();
	InstallPatches();

	Genhook::Initialize();
	CreateDdeServer();

	ResumeThread(hD2Thread);

	Vars.bNeedShutdown = TRUE;

	return TRUE;
}

void Shutdown(void)
{
	if(!Vars.bNeedShutdown)
		return;

	Vars.bActive = false;
	if(!Vars.bShutdownFromDllMain)
		WaitForSingleObject(hD2Thread, INFINITE);

	ShutdownDdeServer();

	delete Vars.image;
	delete Vars.text;

	Console::Destroy();
	Genhook::Destroy();

	UnhookWindowsHookEx(Vars.hMouseHook);
	UnhookWindowsHookEx(Vars.hKeybHook);
	SetWindowLong(D2WIN_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

	RemovePatches();

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
