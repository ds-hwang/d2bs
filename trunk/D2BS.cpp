// Diablo II Botting System Core
#include "D2BS.h"
#include "dde.h"
#include <shlwapi.h>
#include "Offset.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Handlers.h"
#include "Console.h"

#include "debugnew/debug_new.h"

BOOL WINAPI DllMain(HINSTANCE hDll,DWORD dwReason,LPVOID lpReserved)
{
	static HANDLE hD2ThreadHandle = NULL;
	if(dwReason == DLL_PROCESS_ATTACH)
	{
#ifndef _MSVC_DEBUG
		Vars.pModule = (Module*)lpReserved;

		if(!Vars.pModule)
			return FALSE;

		strcpy_s(Vars.szPath, MAX_PATH, Vars.pModule->szPath);
#else 	
		new_verbose_flag = false;
		GetModuleFileName(hDll,Vars.szPath,MAX_PATH);
		PathRemoveFileSpec(Vars.szPath);
		strcat_s(Vars.szPath, MAX_PATH, "\\");
#endif

		if(!Startup())
			return FALSE;

		hD2ThreadHandle = CreateThread(NULL, NULL, D2Thread, NULL, NULL, NULL);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		Shutdown();

		WaitForSingleObject(hD2ThreadHandle, 1000);
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

	DefineOffsets();
	InstallPatches();
	CreateDdeServer();

	return TRUE;
}

void Shutdown(void)
{
	SetWindowLong(D2WIN_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

	ScriptEngine::Shutdown();
	RemovePatches();
	Console::Destroy();
	ShutdownDdeServer();

	Vars.bActive = FALSE;

	delete Vars.image;
	delete Vars.text;

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
}
