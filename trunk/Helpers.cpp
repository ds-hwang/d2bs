#include "D2BS.h"
#include "Core.h"
#include "Script.h"
#include "ScriptEngine.h"
#include "string.h"
#include "D2Handlers.h"
#include "Control.h"
#include "D2Ptrs.h"

wchar_t* AnsiToUnicode(const char* str)
{
	wchar_t* buf = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, buf, 0);
	buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str, -1, buf, len);
	return buf;
}

char* UnicodeToAnsi(const wchar_t* str)
{
	char* buf = NULL;
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, buf, 0, "?", NULL);
	buf = new char[len];
	WideCharToMultiByte(CP_ACP, 0, str, -1, buf, len, "?", NULL);
	return buf;
}

bool StringToBool(const char* str)
{
	switch(tolower(str[0]))
	{
		case 't': case '1':
			return true;
		case 'f': case '0': default:
			return false;
	}
}

void StringReplace(char* str, const char find, const char replace, size_t buflen)
{
	for(size_t i = 0; i < buflen; i++)
	{
		if(str[i] == find)
			str[i] = replace;
	}
}

bool ProfileExists(const char *profile)
{
	char file[_MAX_FNAME+_MAX_PATH];
	char profiles[65535] = "";
	sprintf_s(file, sizeof(file), "%sd2bs.ini", Vars.szPath);

	int count = GetPrivateProfileString(NULL, NULL, NULL, profiles, 65535, file);
	if(count > 0)
	{
		int i = 0;
		while(i < count)
		{
			if(_strcmpi(profiles+i, profile) == 0)
				return true;

			i += strlen(profiles+i)+1;
		}
	}
	return false;
}

void InitSettings(void)
{
	char fname[_MAX_FNAME+MAX_PATH],	scriptPath[_MAX_FNAME+MAX_PATH],
		 debug[6],			blockMinimize[6],		quitOnHostile[6],
		 quitOnError[6],	maxGameTime[6],			gameTimeout[6],
		 startAtMenu[6],	disableCache[6],		memUsage[6],
		 gamePrint[6];

	sprintf_s(fname, sizeof(fname), "%sd2bs.ini", Vars.szPath);

	GetPrivateProfileString("settings", "ScriptPath", "scripts", scriptPath, _MAX_PATH, fname);
	GetPrivateProfileString("settings", "MaxGameTime", "0", maxGameTime, 6, fname);
	GetPrivateProfileString("settings", "Debug", "false", debug, 6, fname);
	GetPrivateProfileString("settings", "BlockMinimize", "false", blockMinimize, 6, fname);
	GetPrivateProfileString("settings", "QuitOnHostile", "false", quitOnHostile, 6, fname);
	GetPrivateProfileString("settings", "QuitOnError", "false", quitOnError, 6, fname);
	GetPrivateProfileString("settings", "StartAtMenu", "true", startAtMenu, 6, fname);
	GetPrivateProfileString("settings", "DisableCache", "true", disableCache, 6, fname);
	GetPrivateProfileString("settings", "MemoryLimit", "50", memUsage, 6, fname);
	GetPrivateProfileString("settings", "UseGamePrint", "false", gamePrint, 6, fname);
	GetPrivateProfileString("settings", "GameReadyTimeout", "5", gameTimeout, 6, fname);

	sprintf_s(Vars.szScriptPath, _MAX_PATH, "%s%s", Vars.szPath, scriptPath);

	Vars.dwGameTime = GetTickCount();
	Vars.dwMaxGameTime = abs(atoi(maxGameTime) * 1000);
	Vars.dwGameTimeout = abs(atoi(gameTimeout) * 1000);
	Vars.bBlockMinimize = StringToBool(blockMinimize);
	Vars.bQuitOnHostile = StringToBool(quitOnHostile);
	Vars.bQuitOnError = StringToBool(quitOnError);
	Vars.bStartAtMenu = StringToBool(startAtMenu);
	Vars.bDisableCache = StringToBool(disableCache);
	Vars.bUseGamePrint = StringToBool(gamePrint);
	Vars.dwMemUsage = abs(atoi(memUsage));
	if(Vars.dwMemUsage < 1)
		Vars.dwMemUsage = 32;
	Vars.dwMemUsage *= 1024*1024;
	Vars.oldWNDPROC = NULL;
}

bool InitHooks(void)
{
	int i = 0;
	while(!Vars.bActive)
	{
		Sleep(50);
		i++;

		if(i >= 300)
		{
			MessageBox(0, "Failed to set hooks, exiting!", "D2BS", 0);
			return false;
		}

		if(D2GFX_GetHwnd() && (ClientState() == ClientStateMenu || ClientState() == ClientStateInGame))
		{
			Vars.oldWNDPROC = (WNDPROC)SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)GameEventHandler);
			if(!Vars.oldWNDPROC)
				continue;

			Vars.uTimer = SetTimer(D2GFX_GetHwnd(), 1, 0, TimerProc);

			DWORD mainThread = GetWindowThreadProcessId(D2GFX_GetHwnd(),0);
			if(mainThread)
			{
				if(!Vars.hKeybHook)
					Vars.hKeybHook = SetWindowsHookEx(WH_KEYBOARD, KeyPress, NULL, mainThread);
				if(!Vars.hMouseHook)
					Vars.hMouseHook = SetWindowsHookEx(WH_MOUSE, MouseMove, NULL, mainThread);
			}
		}
		else
			continue;

		if(Vars.hKeybHook && Vars.hMouseHook)
		{
			if(!ScriptEngine::Startup())
				return false;

			Vars.bActive = TRUE;

			if(ClientState() == ClientStateMenu && Vars.bStartAtMenu)
				clickControl(*p_D2WIN_FirstControl);
		}
	}
	return true;
}

const char* GetStarterScriptName(void)
{
	return (ClientState() == ClientStateInGame ? "default.dbj" : ClientState() == ClientStateMenu ? "starter.dbj" : NULL);
}

ScriptType GetStarterScriptType(void)
{
	// the default return is InGame because that's the least harmful of the options
	return (ClientState() == ClientStateInGame ? InGame : ClientState() == ClientStateMenu ? OutOfGame : InGame);
}

bool ExecCommand(const char* command)
{
	Script* script = ScriptEngine::CompileCommand(command);
	return (script && CreateThread(0, 0, ScriptThread, script, 0, 0) != INVALID_HANDLE_VALUE);
}

bool StartScript(const char* scriptname, ScriptType scriptType)
{
	char file[_MAX_FNAME+_MAX_PATH];
	sprintf_s(file, _MAX_FNAME+_MAX_PATH, "%s\\%s", Vars.szScriptPath, scriptname);
	Script* script = ScriptEngine::CompileFile(file, scriptType);
	return (script && CreateThread(0, 0, ScriptThread, script, 0, 0) != INVALID_HANDLE_VALUE);
}

void Reload(void)
{
	if(ScriptEngine::GetCount() > 0)
		Print("ÿc2D2BSÿc0 :: Stopping all scripts");
	ScriptEngine::StopAll();

	if(Vars.bDisableCache != TRUE)
		Print("ÿc2D2BSÿc0 :: Flushing the script cache");
	ScriptEngine::FlushCache();

	// wait for things to catch up
	Sleep(500);

	const char* script = GetStarterScriptName();
	if(StartScript(script, GetStarterScriptType()))
		Print("ÿc2D2BSÿc0 :: Started %s", script);
	else
		Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
}

bool ProcessCommand(const char* command, bool unprocessedIsCommand)
{
	bool result = false;
	char* buf = _strdup(command);
	char* next_token1;
	char* argv = strtok_s(buf, " ", &next_token1);

	// no command?
	if(argv == NULL)
		return false;

	if(_strcmpi(argv, "start") == 0)
	{
		const char* script = GetStarterScriptName();
		if(StartScript(script, GetStarterScriptType()))
			Print("ÿc2D2BSÿc0 :: Started %s", script);
		else
			Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
		result = true;
	}
	else if(_strcmpi(argv, "stop") == 0)
	{
		if(ScriptEngine::GetCount() > 0)
			Print("ÿc2D2BSÿc0 :: Stopping all scripts");
		ScriptEngine::StopAll();
		result = true;
	}
	else if(_strcmpi(argv, "flush") == 0)
	{
		if(Vars.bDisableCache != TRUE)
			Print("ÿc2D2BSÿc0 :: Flushing the script cache");
		ScriptEngine::FlushCache();
		result = true;
	}
	else if(_strcmpi(argv, "load") == 0)
	{
		const char* script = command+5;
		if(StartScript(script, GetStarterScriptType()))
			Print("ÿc2D2BSÿc0 :: Started %s", script);
		else
			Print("ÿc2D2BSÿc0 :: Failed to start %s", script);
		result = true;
	}
	else if(_strcmpi(argv, "reload") == 0)
	{
		Reload();
		result = true;
	}
	else if(_strcmpi(argv, "exec") == 0 && !unprocessedIsCommand)
	{
		ExecCommand(command+5);
		result = true;
	}
	else if(unprocessedIsCommand)
	{
		ExecCommand(command);
		result = true;
	}
	return result;
}

void GameJoined(void)
{
	Print("ÿc2D2BSÿc0 :: Starting default.dbj");
	if(StartScript(GetStarterScriptName(), GetStarterScriptType()))
		Print("ÿc2D2BSÿc0 :: default.dbj running.");
	else
		Print("ÿc2D2BSÿc0 :: Failed to start default.dbj!");
}

void MenuEntered(bool beginStarter)
{
	if(beginStarter)
	{
		Print("ÿc2D2BSÿc0 :: Starting starter.dbj");
		if(StartScript(GetStarterScriptName(), GetStarterScriptType()))
			Print("ÿc2D2BSÿc0 :: starter.dbj running.");
		else
			Print("ÿc2D2BSÿc0 :: Failed to start starter.dbj!");
	}
}
