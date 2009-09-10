// Diablo II Botting System Core
#include "D2BS.h"
#include "dde.h"
#include "shlwapi.h"
#include "Offset.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Handlers.h"
#include "Console.h"

#include "debugnew/debug_new.h"

#include <windows.h>

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
		strcat(Vars.szPath,"\\");
#endif

		Startup();

		hD2ThreadHandle = CreateThread(NULL, NULL, D2Thread, NULL, NULL, NULL);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		Shutdown();

		WaitForSingleObject(hD2ThreadHandle, 1000);
	}

	return TRUE;
}

void Startup(void)
{
	char path[_MAX_FNAME+MAX_PATH],
		 fname[_MAX_FNAME+MAX_PATH],
		 scriptPath[_MAX_FNAME+MAX_PATH],
		 debug[6],
		 blockMinimize[6],
		 quitOnHostile[6],
		 quitOnError[6],
		 maxGameTime[6],
		 startAtMenu[6],
		 disableCache[6],
		 memUsage[6];

	sprintf(path, "%sd2bs.log", Vars.szPath);
	sprintf(fname, "%sd2bs.ini", Vars.szPath);

	if(freopen(path, "a+t", stderr) == NULL)
		MessageBox(0, "Failed to redirect output!", "D2BS", 0);

	GetPrivateProfileString("settings", "ScriptPath", "scripts", scriptPath, _MAX_PATH, fname);
	GetPrivateProfileString("settings", "MaxGameTime", "0", maxGameTime, 6, fname);
	GetPrivateProfileString("settings", "Debug", "false", debug, 6, fname);
	GetPrivateProfileString("settings", "BlockMinimize", "false", blockMinimize, 6, fname);
	GetPrivateProfileString("settings", "QuitOnHostile", "false", quitOnHostile, 6, fname);
	GetPrivateProfileString("settings", "QuitOnError", "false", quitOnError, 6, fname);
	GetPrivateProfileString("settings", "StartAtMenu", "true", startAtMenu, 6, fname);
	GetPrivateProfileString("settings", "DisableCache", "false", disableCache, 6, fname);
	GetPrivateProfileString("settings", "MemoryLimit", "50", memUsage, 6, fname);

	sprintf(Vars.szScriptPath, "%s%s", Vars.szPath, scriptPath);
	Vars.dwGameTime = 0;
	Vars.dwMaxGameTime = atoi(maxGameTime);
	Vars.bDebug = StringToBool(debug);
	Vars.bBlockMinimize = StringToBool(blockMinimize);
	Vars.bQuitOnHostile = StringToBool(quitOnHostile);
	Vars.bQuitOnError = StringToBool(quitOnError);
	Vars.bStartAtMenu = StringToBool(startAtMenu);
	Vars.bDisableCache = StringToBool(disableCache);
	Vars.dwMemUsage = atoi(memUsage);
	if(Vars.dwMemUsage < 1)
		Vars.dwMemUsage = 50;
	Vars.dwMemUsage *= 1024*1024;

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

	Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, GetCurrentThreadId());
	if(Vars.hMouseHook == NULL)
		Log("SetWindowsHookEx returned error 0x%x when attempting to set a mouse hook", GetLastError());

	Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, GetCurrentThreadId());
	if(Vars.hKeybHook == NULL)
		Log("SetWindowsHookEx returned error 0x%x when attempting to set a keyboard hook", GetLastError());

	DefineOffsets();
	InstallPatches();
	CreateDdeServer();
	ScriptEngine::Startup();

	Vars.bActive = TRUE;
	Vars.oldWNDPROC = NULL;

	char versionimg[_MAX_PATH+_MAX_FNAME];
	sprintf(versionimg, "%sversion.bmp", Vars.szPath);
	Vars.image = new ImageHook(NULL, versionimg, 0, 10, 0, false, Center, Perm);
	Vars.text = new TextHook(NULL, "D2BS " D2BS_VERSION, 0, 15, 13, 4, false, Center, Perm);
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
