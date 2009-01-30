// Diablo II Botting System Core
#include "D2BS.h"
#include "Script.h"
#include "Helpers.h"

Variables Vars;

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
		GetModuleFileName(hDll,Vars.szPath,MAX_PATH);
		PathRemoveFileSpecA(Vars.szPath);
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

		Vars.oldWNDPROC = NULL;
		Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, GetCurrentThreadId());
		Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, GetCurrentThreadId());
		hD2ThreadHandle = CreateThread(NULL, NULL, D2Thread, NULL, NULL, NULL);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		SetWindowLong(D2WIN_GetHwnd(),GWL_WNDPROC,(LONG)Vars.oldWNDPROC);

		RemovePatches();
		Script::Shutdown();

		ShutdownDdeServer();

		TerminateThread(hD2ThreadHandle, NULL);
		UnhookWindowsHookEx(Vars.hMouseHook);
		UnhookWindowsHookEx(Vars.hKeybHook);

		DeleteCriticalSection(&Vars.cRoomSection);
		DeleteCriticalSection(&Vars.cMiscSection);
		DeleteCriticalSection(&Vars.cScreenhookSection);
		DeleteCriticalSection(&Vars.cPrintSection);
	}

	return TRUE;
}
