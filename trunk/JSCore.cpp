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
//#include "D2BS.h"
#include "Events.h"
#include "Console.h"
#include "D2Ptrs.h"

JSAPI_FUNC(my_print)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]))
		{
			if(!JSVAL_IS_STRING(argv[i]) && !JS_ConvertValue(cx, argv[i], JSTYPE_STRING, &(argv[i])))
				THROW_ERROR(cx, "Could not convert to string");

			char* Text = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			if(Text == NULL)
				THROW_ERROR(cx, "Could not get string for value");

			jsrefcount depth = JS_SuspendRequest(cx);
			if(strlen(Text) == 0)
				Print("undefined");
			else
			{
				StringReplace(Text, '%', (unsigned char)0xFE, strlen(Text));
				Print(Text);
			}
			JS_ResumeRequest(cx, depth);
		}
	}
	return JS_TRUE;
}

JSAPI_FUNC(my_delay)
{
	uint32 nDelay = 0;
	if(!JS_ConvertArguments(cx, argc, argv, "u", &nDelay))
		THROW_ERROR(cx, "Could not convert arguments")

	if(nDelay == 0)
		JS_ReportWarning(cx, "delay(0) called, argument must be >= 1, doing delay(1)");
	else if(nDelay < 50)
	{
		jsrefcount depth = JS_SuspendRequest(cx);
		Sleep(nDelay);
		JS_ResumeRequest(cx, depth);
	}
	else
	{
		DWORD StartTick = GetTickCount();
		while(GetTickCount() - StartTick < (DWORD)nDelay)
		{
			jsrefcount depth = JS_SuspendRequest(cx);
			Sleep(50);
			JS_ResumeRequest(cx, depth);
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_load)
{
	*rval = JSVAL_FALSE;

	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(!script)
		THROW_ERROR(cx, "Failed to get script object");

	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s", &file))
		THROW_ERROR(cx, "Failed to convert arugments");

	if(strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath)))
		THROW_ERROR(cx, "File name is too long");

	char buf[_MAX_PATH+_MAX_FNAME];
	ScriptType scriptType = script->GetScriptType();
	if(scriptType == Command)
		scriptType = ClientState() == ClientStateInGame ? InGame : OutOfGame;

	sprintf_s(buf, sizeof(buf), "%s\\%s", Vars.szScriptPath, file);
	
	_strlwr_s(buf, _MAX_PATH+_MAX_FNAME);
	StringReplace(buf, '/', '\\', _MAX_PATH+_MAX_FNAME);
	Script* newScript = ScriptEngine::CompileFile(buf, scriptType);
	if(newScript)
	{
		CreateThread(0, 0, ScriptThread, newScript, 0, 0);
		*rval = JSVAL_TRUE;
	}
	else
	{
		char msg[_MAX_PATH + _MAX_FNAME + 20];
		sprintf_s(msg, _MAX_PATH+_MAX_FNAME+20, "File \"%s\" not found.", file);
		THROW_ERROR(cx, msg);
	}

	return JS_TRUE;
}

JSAPI_FUNC(my_include)
{
	*rval = JSVAL_FALSE;

	Script* script = (Script*)JS_GetContextPrivate(cx);
	if(!script)
		THROW_ERROR(cx, "Failed to get script object");

	char* file = NULL;
	if(!JS_ConvertArguments(cx, argc, argv, "s", &file))
		THROW_ERROR(cx, "Failed to convert arguments");

	if(strlen(file) > (_MAX_FNAME + _MAX_PATH - strlen(Vars.szScriptPath) - 6))
		THROW_ERROR(cx, "File name is too long.");

	char buf[_MAX_PATH+_MAX_FNAME];
	sprintf_s(buf, sizeof(buf), "%s\\libs\\%s", Vars.szScriptPath, file);

	if(_access(buf, 0) == 0)
		*rval = BOOLEAN_TO_JSVAL(script->Include(buf));

	return JS_TRUE;
}

// TODO FIXME AUDIT
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

	return JS_TRUE;
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
		THROW_ERROR(cx, "Failed to convert arguments");

	if(strlen(file) > (_MAX_FNAME+_MAX_PATH-strlen(Vars.szScriptPath)-6))
		THROW_ERROR(cx, "File name is too long");

	char path[_MAX_FNAME+_MAX_PATH];
	sprintf_s(path, _MAX_FNAME+_MAX_PATH, "%s\\libs\\%s", Vars.szScriptPath, file);
	Script* script = (Script*)JS_GetContextPrivate(cx);
	*rval = BOOLEAN_TO_JSVAL(script->IsIncluded(path));

	return JS_TRUE;
}

JSAPI_FUNC(my_version)
{
	*rval = STRING_TO_JSVAL(JS_InternString(cx, D2BS_VERSION));
	return JS_TRUE;
}

JSAPI_FUNC(my_debugLog)
{
	for(uintN i = 0; i < argc; i++)
	{
		if(!JSVAL_IS_NULL(argv[i]))
		{
			if(!JS_ConvertValue(cx, argv[i], JSTYPE_STRING, &(argv[i])))
				THROW_ERROR(cx, "Converting to string failed");

			char* Text = JS_GetStringBytes(JS_ValueToString(cx, argv[i]));
			if(!Text)
				THROW_ERROR(cx, "Could not get string for value");

			jsrefcount depth = JS_SuspendRequest(cx);
			if(strlen(Text) == 0)
				Log("undefined");
			else
			{
				StringReplace(Text, '%', (unsigned char)0xFE, strlen(Text));
				Log(Text);
			}
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
	bool resetClassName = !!!JSVAL_IS_STRING(argv[0]), resetName = !!!JSVAL_IS_STRING(argv[1]);

	if(!JS_ConvertArguments(cx, argc, argv, "ssis", &windowClassName, &windowName, &nModeId, &data))
		THROW_ERROR(cx, "Could not convert arguments");

	if(resetClassName)
		windowClassName = NULL;
	if(resetName)
		windowName = NULL;

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
	// HACK: Using PostMessage instead of SendMessage--need to fix this ASAP!
	*rval = INT_TO_JSVAL(SendMessage(hWnd, WM_COPYDATA, (WPARAM)D2GFX_GetHwnd(), (LPARAM)&aCopy));

	return JS_TRUE;
}

JSAPI_FUNC(my_sendDDE)
{
	jsint mode;
	char *pszDDEServer = "\"\"", *pszTopic = "\"\"", *pszItem = "\"\"", *pszData = "\"\"";

	if(!JS_ConvertArguments(cx, argc, argv, "isss", &mode, &pszDDEServer, &pszTopic, &pszItem))
		THROW_ERROR(cx, "Could not convert arguments");

	char buffer[255] = "";
	if(SendDDE(mode, pszDDEServer, pszTopic, pszItem, pszData, (char**)&buffer, 255))
		if(mode == 0)
			*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, buffer));

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

JSAPI_FUNC(my_clearConsole)
{
	Console::Clear();
	return JS_TRUE;
}
