#include <io.h>
#include <windows.h>
#include <ddeml.h>

#include "js32.h"
#include "Script.h"
#include "JSCore.h"
#include "Core.h"
#include "Helpers.h"
#include "dde.h"
#include "ScriptEngine.h"
#include "D2BS.h"
#include "Events.h"
#include "Console.h"

JSAPI_FUNC(my_print)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]))
		{
			if(!JS_ConvertValue(cx, argv[i], JSTYPE_STRING, &(argv[i])))
			{
				JS_ReportError(cx, "Converting to string failed");
				return JS_FALSE;
			}

			char* Text = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			if(Text == NULL)
			{
				JS_ReportError(cx, "Could not get string for value");
				return JS_FALSE;
			}

			jsrefcount depth = JS_SuspendRequest(cx);
			StringReplace(Text, '%', (unsigned char)0xFE);
			Print(Text ? Text : "undefined");
			JS_ResumeRequest(cx, depth);
		}
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_delay)
{
	uint32 nDelay = 0;
	if(!JS_ConvertArguments(cx, argc, argv, "u", &nDelay))
		return JS_FALSE;

	if(nDelay)
	{
		jsrefcount depth = JS_SuspendRequest(cx);
		Sleep(nDelay);
		JS_ResumeRequest(cx, depth);
	}
	else
		JS_ReportWarning(cx, "delay(0) called, argument must be >= 1");

/*	if(argc == 1 && JSVAL_IS_INT(argv[0]))
	{
		int nDelay = JSVAL_TO_INT(argv[0]);
		if(nDelay)
		{
			jsrefcount depth = JS_SuspendRequest(cx);

			Sleep(nDelay);

			JS_ResumeRequest(cx, depth);
		}
		else
			JS_ReportWarning(cx, "delay(0) called, argument must be >= 1");
	}*/

	return JS_TRUE;
}

JSAPI_FUNC(my_load)
{
	*rval = JSVAL_FALSE;

	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(!script)
	{
		JS_ReportError(cx, "Failed to get script object");
		return JS_FALSE;
	}

	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s", &file))
		return JS_FALSE;

	if(strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath)))
	{
		JS_ReportError(cx, "File name too long!");
		return JS_FALSE;
	}

	char buf[_MAX_PATH+_MAX_FNAME];
	ScriptState scriptState = script->GetState();
	if(scriptState == Command)
		scriptState = (GameReady() ? InGame : OutOfGame);

	sprintf_s(buf, sizeof(buf), "%s\\%s", Vars.szScriptPath, file);
	StringReplace(buf, '/', '\\');
	Script* newScript = ScriptEngine::CompileFile(buf, scriptState);
	if(newScript)
	{
		CreateThread(0, 0, ScriptThread, newScript, 0, 0);
		*rval = JSVAL_TRUE;
	}
	else
	{
		// TODO: Should this actually be there? No notification is bad, but do we want this? maybe throw an exception?
		Print("File \"%s\" not found.", file);
		*rval = JSVAL_FALSE;
	}

/*	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		Script* execScript = (Script*)JS_GetContextPrivate(cx);
		ScriptState scriptState = execScript->GetState();
		if(scriptState == Command)
			scriptState = (GameReady() ? InGame : OutOfGame);

		char* lpszFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(!(lpszFileName && lpszFileName[0]))
			THROW_ERROR(cx, obj, "Could not convert or empty string");
		StringReplace(lpszFileName, '/', '\\');
		if(strlen(lpszFileName) < (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath)))
		{
			char lpszBuf[_MAX_PATH+_MAX_FNAME];
			sprintf_s(lpszBuf, sizeof(lpszBuf), "%s\\%s", Vars.szScriptPath, lpszFileName);
			Script* script = ScriptEngine::CompileFile(lpszBuf, scriptState);
			if(script)
			{
				CreateThread(0, 0, ScriptThread, script, 0, 0);
				*rval = JSVAL_TRUE;
			}
			else
			{
				// TODO: Should this actually be there? No notification is bad, but do we want this? maybe throw an exception?
				Print("File \"%s\" not found.", lpszFileName);
				*rval = JSVAL_FALSE;
			}
		}
		else
			THROW_ERROR(cx, obj, "File name exceeds max name length");
	}*/

	return JS_TRUE;
}

JSAPI_FUNC(my_include)
{
	*rval = JSVAL_FALSE;

	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(!script)
	{
		JS_ReportError(cx, "Failed to get script object");
		return JS_FALSE;
	}

	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s", &file))
		return JS_FALSE;

	if(strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath) - 6))
	{
		JS_ReportError(cx, "File name too long!");
		return JS_FALSE;
	}

	char buf[_MAX_PATH+_MAX_FNAME];
	sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);
	if(_access(buf, 0) == 0)
		*rval = BOOLEAN_TO_JSVAL(script->Include(buf));

/*	if(argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		if(script)
		{
			char * lpszFileName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
			if(lpszFileName && strlen(lpszFileName) <= _MAX_FNAME)
			{
				char lpszBuf[_MAX_PATH+_MAX_FNAME];
				sprintf_s(lpszBuf, sizeof(lpszBuf), "%s\\libs\\%s", Vars.szScriptPath, lpszFileName);
				if(_access(lpszBuf, 0) == 0)
					*rval = BOOLEAN_TO_JSVAL(script->Include(lpszBuf));
			}
		}
	}*/

	return JS_TRUE;
}

JSAPI_FUNC(my_stop)
{
	if(argc > 0 && (JSVAL_IS_INT(argv[0]) && JSVAL_TO_INT(argv[0]) == 1) ||
			(JSVAL_IS_BOOLEAN(argv[0]) && JSVAL_TO_BOOLEAN(argv[0]) == TRUE))
	{
		Script* script = (Script*)JS_GetContextPrivate(cx);
		if(script)
			script->Stop();
	}
	else
		ScriptEngine::StopAll();

	return JS_FALSE;
}

JSAPI_FUNC(my_beep)
{
	jsint nBeepId = NULL;

	if(argc > 0 && JSVAL_IS_INT(argv[0]))
		nBeepId = JSVAL_TO_INT(argv[0]);

	MessageBeep(nBeepId);

	return JS_TRUE;
}

JSAPI_FUNC(my_blockMinimize)
{
	if(argc > 0 && (JSVAL_IS_INT(argv[0]) || JSVAL_IS_BOOLEAN(argv[0])))
		Vars.bBlockMinimize = !!JSVAL_TO_BOOLEAN(argv[0]);

	return JS_TRUE;
}

JSAPI_FUNC(my_getTickCount)
{
	*rval = INT_TO_JSVAL(GetTickCount());
	return JS_TRUE;
}

JSAPI_FUNC(my_getThreadPriority)
{
	*rval = GetThreadPriority(GetCurrentThread());
	return JS_TRUE;
}

JSAPI_FUNC(my_isIncluded)
{
	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s", &file))
		return JS_FALSE;

	if(strlen(file) > (_MAX_FNAME+_MAX_PATH-strlen(Vars.szScriptPath)-6))
	{
		JS_ReportError(cx, "File name too long");
		return JS_FALSE;
	}

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\libs\\%s", Vars.szScriptPath, file);
	Script* script = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(script->IsIncluded(path));

/*	if(argc < 1 || !JSVAL_IS_STRING(argv[0]))
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	char* szFile = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	if(!(szFile && szFile[0]))
		THROW_ERROR(cx, obj, "Could not convert string");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\libs\\%s", Vars.szScriptPath, szFile);
	Script* script = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(script->IsIncluded(path));*/

	return JS_TRUE;
}

JSAPI_FUNC(my_version)
{
	if(argc < 1)
	{
		*rval = STRING_TO_JSVAL(JS_InternString(cx, D2BS_VERSION));
		return JS_TRUE;
	}

	Print("�c4D2BS�c1 �c3%s for Diablo II 1.12a.", D2BS_VERSION); 

	return JS_TRUE;
}

JSAPI_FUNC(my_debugLog)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]))
		{
			if(!JS_ConvertValue(cx, argv[i], JSTYPE_STRING, &(argv[i])))
			{
				JS_ReportError(cx, "Converting to string failed");
				return JS_FALSE;
			}

			char* Text = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			if(Text == NULL)
			{
				JS_ReportError(cx, "Could not get string for value");
				return JS_FALSE;
			}

			jsrefcount depth = JS_SuspendRequest(cx);
			StringReplace(Text, '%', (unsigned char)0xFE);
			Log(Text ? Text : "undefined");
			JS_ResumeRequest(cx, depth);
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_sendCopyData)
{
	if(argc < 4)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	char *windowClassName = NULL, *windowName = NULL, *data = NULL;
	jsint nModeId = NULL;

	if(!JS_ConvertArguments(cx, argc, argv, "ssis", &windowClassName, &windowName, &nModeId, &data))
		return JS_FALSE;

	if(_strcmpi(windowClassName, "null") == 0)
		windowClassName = NULL;
	if(_strcmpi(windowName, "null") == 0)
		windowName = NULL;

/*	if(JSVAL_IS_STRING(argv[0]))
	{
		windowClassName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		if(!windowClassName)
			THROW_ERROR(cx, obj, "Could not convert string");
	}
	
	if(JSVAL_IS_STRING(argv[1]))
	{
		windowName = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		if(!windowName)
			THROW_ERROR(cx, obj, "Could not convert string");
	}
	
	if(JSVAL_IS_INT(argv[2]))
	{
		if(JS_ValueToInt32(cx, argv[2], &nModeId) == JS_FALSE)
			THROW_ERROR(cx, obj, "Could not convert value");
	}
	
	if(JSVAL_IS_STRING(argv[3]))
	{
		data = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
		if(!data)
			THROW_ERROR(cx, obj, "Could not convert string");
	}*/

	HWND hWnd = FindWindow(windowClassName, windowName);
	if(!hWnd)
	{
		*rval = JSVAL_FALSE;
		return JS_TRUE;
	}

	// if data is NULL, strlen crashes
	if(data == NULL)
		data = "";

	COPYDATASTRUCT aCopy = { nModeId, strlen(data)+1, data };
	*rval = INT_TO_JSVAL(SendMessage(hWnd, WM_COPYDATA, (WPARAM)D2WIN_GetHwnd(), (LPARAM)&aCopy));

	return JS_TRUE;
}

JSAPI_FUNC(my_sendDDE)
{
	/*if(argc > 4 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_STRING(argv[1]) && JSVAL_IS_STRING(argv[2])
		&& JSVAL_IS_STRING(argv[3]) && JSVAL_IS_STRING(argv[4]))
		return JS_TRUE;*/

	jsint mode;
	char *pszDDEServer = "\"\"", *pszTopic = "\"\"", *pszItem = "\"\"", *pszData = "\"\"";

	if(!JS_ConvertArguments(cx, argc, argv, "isss", &mode, &pszDDEServer, &pszTopic, &pszItem))
		return JS_FALSE;

	char buffer[255] = "";
	if(SendDDE(mode, pszDDEServer, pszTopic, pszItem, pszData, (char**)&buffer, 255))
		if(mode == 0)
			*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, buffer));

	/*if(JS_ValueToInt32(cx, argv[0], &mode) == JS_FALSE)
		THROW_ERROR(cx, obj, "Could not convert value");
	char *pszDDEServer = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	if(!strlen(pszDDEServer))
		pszDDEServer = "\"\"";
	char *pszTopic = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	if(!strlen(pszTopic))
		pszTopic = "\"\"";
	char *pszItem = JS_GetStringBytes(JS_ValueToString(cx, argv[3]));
	if(!strlen(pszItem))
		pszItem = "\"\"";
	char *pszData = JS_GetStringBytes(JS_ValueToString(cx, argv[4]));
	if(!strlen(pszData))
		pszData = "\"\"";*/

	return JS_TRUE;
}

JSAPI_FUNC(my_keystate)
{
	if(argc < 1 || !JSVAL_IS_INT(argv[0]))
		return JS_TRUE;
	
	*rval = BOOLEAN_TO_JSVAL(!!GetAsyncKeyState(JSVAL_TO_INT(argv[0])));

	return JS_TRUE;
}

JSAPI_FUNC(my_addEventListener)
{
	if(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_FUNCTION(cx, argv[1]))
	{
		char* evtName = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		if(evtName && strlen(evtName))
		{
			Script* self = (Script*)JS_GetContextPrivate(cx);
			self->RegisterEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])), argv[1]);
		}
		else
			THROW_ERROR(cx, "Event name is invalid!");
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_removeEventListener)
{
	if(JSVAL_IS_STRING(argv[0]) && JSVAL_IS_FUNCTION(cx, argv[1]))
	{
		char* evtName = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
		if(evtName && strlen(evtName))
		{
			Script* self = (Script*)JS_GetContextPrivate(cx);
			self->UnregisterEvent(evtName, argv[1]);
		}
		else
			THROW_ERROR(cx, "Event name is invalid!");
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearEvent)
{
	if(JSVAL_IS_STRING(argv[0]))
	{
		Script* self = (Script*)JS_GetContextPrivate(cx);
		self->ClearEvent(JS_GetStringBytes(JSVAL_TO_STRING(argv[0])));
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_clearAllEvents)
{
	Script* self = (Script*)JS_GetContextPrivate(cx);
	self->ClearAllEvents();
	return JS_TRUE;
}

JSAPI_FUNC(my_js_strict)
{
	if(argc == NULL)
	{
		*rval = BOOLEAN_TO_JSVAL(((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT));
		return JS_TRUE;
	}

	if(argc == 1)
	{
		bool bFlag = ((JS_GetOptions(cx) & JSOPTION_STRICT) == JSOPTION_STRICT);
		if(JSVAL_TO_BOOLEAN(argv[0]))
		{
			if(!bFlag)
				JS_ToggleOptions(cx, JSOPTION_STRICT);
		}
		else
		{
			if(bFlag)
				JS_ToggleOptions(cx, JSOPTION_STRICT);
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_scriptBroadcast)
{
	if(argc < 1)
		THROW_ERROR(cx, "You must specify something to broadcast");

	ScriptBroadcastEvent(argc, argv);
	return JS_TRUE;
}

JSAPI_FUNC(my_showConsole)
{
	Console::Show();
	return JS_TRUE;
}

JSAPI_FUNC(my_hideConsole)
{
	Console::Hide();
	return JS_TRUE;
}
