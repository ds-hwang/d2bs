// Diablo II Botting System Core
#include "D2BS.h"
#include "dde.h"
#include "shlwapi.h"
#include "Offset.h"
#include "Script.h"
#include "Helpers.h"
#include "D2Handlers.h"

#include "debugnew/debug_new.h"

#include <windows.h>

BOOL WINAPI DllMain(HINSTANCE hDll,DWORD dwReason,LPVOID lpReserved)
{
	new_verbose_flag = false;
	static HANDLE hD2ThreadHandle = NULL;
	if(dwReason == DLL_PROCESS_ATTACH)
	{
#ifndef _MSVC_DEBUG
		Vars.pModule = (Module*)lpReserved;

		if(!Vars.pModule)
			return FALSE;

		strcpy_s(Vars.szPath, MAX_PATH, Vars.pModule->szPath);
#else 	
		GetModuleFileName(hDll,Vars.szPath,MAX_PATH);
		PathRemoveFileSpec(Vars.szPath);
		strcat(Vars.szPath,"\\");
#endif

		char path[_MAX_FNAME+MAX_PATH],
			 fname[_MAX_FNAME+MAX_PATH],
			 scriptPath[_MAX_FNAME+MAX_PATH],
			 debug[6],
			 blockMinimize[6],
			 quitOnHostile[6],
			 quitOnError[6],
			 maxGameTime[6],
			 startAtMenu[6];

		sprintf(path, "%sD2BS.log", Vars.szPath);
		sprintf(fname, "%sd2bs.ini", Vars.szPath);

		freopen(path, "a+", stderr);

		GetPrivateProfileString("settings", "ScriptPath", "scripts", scriptPath, _MAX_PATH, fname);
		GetPrivateProfileString("settings", "MaxGameTime", "0", maxGameTime, 6, fname);
		GetPrivateProfileString("settings", "Debug", "false", debug, 6, fname);
		GetPrivateProfileString("settings", "BlockMinimize", "false", blockMinimize, 6, fname);
		GetPrivateProfileString("settings", "QuitOnHostile", "false", quitOnHostile, 6, fname);
		GetPrivateProfileString("settings", "QuitOnError", "false", quitOnError, 6, fname);
		GetPrivateProfileString("settings", "StartAtMenu", "true", startAtMenu, 6, fname);

		sprintf(Vars.szScriptPath, "%s%s", Vars.szPath, scriptPath);
		Vars.dwGameTime = 0;
		Vars.dwMaxGameTime = atoi(maxGameTime);
		Vars.bDebug = StringToBool(debug);
		Vars.bBlockMinimize = StringToBool(blockMinimize);
		Vars.bQuitOnHostile = StringToBool(quitOnHostile);
		Vars.bQuitOnError = StringToBool(quitOnError);
		Vars.bStartAtMenu = StringToBool(startAtMenu);

		InitializeCriticalSection(&Vars.cRoomSection);
		InitializeCriticalSection(&Vars.cMiscSection);
		InitializeCriticalSection(&Vars.cScreenhookSection);
		InitializeCriticalSection(&Vars.cPrintSection);

		DefineOffsets();
		InstallPatches();
		Script::Startup();

		CreateDdeServer();

		Vars.bActive = TRUE;
		Vars.oldWNDPROC = NULL;
		Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, GetCurrentThreadId());
		Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, GetCurrentThreadId());

		char versionimg[_MAX_PATH+_MAX_FNAME];
		sprintf(versionimg, "%s\\version.bmp", Vars.szPath);
		Vars.image = new ImageHook(NULL, versionimg, 0, 10, 0, false, Center, Perm);
		Vars.text = new TextHook(NULL, "D2BS " D2BS_VERSION, 0, 15, 13, 4, false, Center, Perm);

		hD2ThreadHandle = CreateThread(NULL, NULL, D2Thread, NULL, NULL, NULL);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		SetWindowLong(D2WIN_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

		RemovePatches();
		Script::Shutdown();

		ShutdownDdeServer();

		UnhookWindowsHookEx(Vars.hMouseHook);
		UnhookWindowsHookEx(Vars.hKeybHook);

		Vars.bActive = FALSE;
		WaitForSingleObject(hD2ThreadHandle, 1000);

		delete Vars.image;
		delete Vars.text;

		DeleteCriticalSection(&Vars.cRoomSection);
		DeleteCriticalSection(&Vars.cMiscSection);
		DeleteCriticalSection(&Vars.cScreenhookSection);
		DeleteCriticalSection(&Vars.cPrintSection);
	}

	return TRUE;
}
