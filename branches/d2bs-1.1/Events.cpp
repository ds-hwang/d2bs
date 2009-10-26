#include "ScriptEngine.h"

#include "debugnew/debug_new.h"

VOID ChatEvent(CHAR* lpszNick, CHAR* lpszMsg)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszNick)));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("chatmsg", 2, argv);
	}
}

VOID WhisperEvent(CHAR* lpszNick, CHAR* lpszMsg)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszNick)));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("whispermsg", 2, argv);
	}
}

VOID LifeEvent(DWORD dwLife, DWORD dwMana)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(INT_TO_JSVAL(dwLife));
		argv[1] = new AutoRoot(INT_TO_JSVAL(dwMana));
		(*it)->ExecEventAsync("melife", 2, argv);
	}
}

VOID CopyDataEvent(DWORD dwMode, CHAR* lpszMsg)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(INT_TO_JSVAL(dwMode));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("copydata", 2, argv);
	}
}

VOID GameMsgEvent(CHAR* lpszMsg)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
		(*it)->ExecEventAsync("gamemsg", 1, argv);
	}
}

VOID KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(INT_TO_JSVAL(key));
		(*it)->ExecEventAsync((bUp ? "keyup" : "keydown"), 1, argv);
	}
}

VOID PlayerAssignEvent(DWORD dwUnitId)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(INT_TO_JSVAL(dwUnitId));
		(*it)->ExecEventAsync("playerassign", 1, argv);
	}
}

VOID MouseClickEvent(int button, POINT pt, bool bUp)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		AutoRoot** argv = new AutoRoot*[3];
		argv[0] = new AutoRoot(INT_TO_JSVAL(button));
		argv[1] = new AutoRoot(INT_TO_JSVAL(pt.x));
		argv[2] = new AutoRoot(INT_TO_JSVAL(pt.y));
		(*it)->ExecEventAsync((bUp ? "mouseup" : "mousedown"), 3, argv);
	}
}

VOID MouseMoveEvent(POINT pt)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(INT_TO_JSVAL(pt.x));
		argv[1] = new AutoRoot(INT_TO_JSVAL(pt.y));
		(*it)->ExecEventAsync("mousemove", 2, argv);
	}
}

VOID ScriptBroadcastEvent(uintN argc, jsval* args)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		AutoRoot** argv = new AutoRoot*[argc];
		for(uintN i = 0; i < argc; i++)
			argv[i] = new AutoRoot(args[i]);
		(*it)->ExecEventAsync("scriptmsg", argc, argv);
	}
}
VOID ItemDropEvent(DWORD GID, CHAR* Code, WORD itemX, WORD itemY, WORD Mode)
{
	ScriptList scripts;
	ScriptEngine::GetScripts(scripts);
	for(ScriptList::iterator it = scripts.begin(); it != scripts.end(); it++)
	{
		if(!(*it)->IsRunning())
			continue;
		JSContext* cx = (*it)->GetContext();
		AutoRoot** argv = new AutoRoot*[5];
		argv[0] = new AutoRoot(INT_TO_JSVAL(GID));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, Code)));
		argv[2] = new AutoRoot(INT_TO_JSVAL(itemX));
		argv[3] = new AutoRoot(INT_TO_JSVAL(itemY));
		argv[4] = new AutoRoot(INT_TO_JSVAL(Mode));
		(*it)->ExecEventAsync("itemdrop", 5, argv);
	}
}
