#include "ScriptEngine.h"

struct ChatEventHelper
{
	char *event, *nick, *msg;
};

struct CopyDataHelper
{
	DWORD mode;
	char* msg;
};

struct ItemEventHelper
{
	DWORD id;
	char* code;
	WORD mode;
	bool global;
};

struct KeyEventHelper
{
	BOOL up;
	WPARAM key;
};

struct GameActionEventHelper
{
	BYTE mode;
	DWORD param;
	char *name1, *name2;
};

struct SingleArgHelper
{
	DWORD arg1;
};

struct DoubleArgHelper
{
	DWORD arg1, arg2;
};

struct TripleArgHelper
{
	DWORD arg1, arg2, arg3;
};

struct QuadArgHelper
{
	DWORD arg1, arg2, arg3, arg4;
};

struct BCastEventHelper
{
	jsval* argv;
	uintN argc;
};

bool __fastcall LifeEventCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("melife"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		script->ExecEventAsync("melife", 1, argv);
	}
	return true;
}

void LifeEvent(DWORD dwLife)
{
	SingleArgHelper helper = {dwLife};
	ScriptEngine::ForEachScript(LifeEventCallback, &helper, 1);
}

bool __fastcall ManaEventCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("memana"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		script->ExecEventAsync("memana", 1, argv);
	}
	return true;
}

void ManaEvent(DWORD dwMana)
{
	SingleArgHelper helper = {dwMana};
	ScriptEngine::ForEachScript(ManaEventCallback, &helper, 1);
}

bool __fastcall KeyEventCallback(Script* script, void* argv, uint argc)
{
	KeyEventHelper* helper = (KeyEventHelper*)argv;
	char* event = (helper->up ? "keyup" : "keydown");
	if(script->IsRunning() && script->IsListenerRegistered(event))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->key));
		script->ExecEventAsync(event, 1, argv);
	}
	return true;
}

void KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	KeyEventHelper helper = {bUp, key};
	ScriptEngine::ForEachScript(KeyEventCallback, &helper, 1);
}

bool __fastcall PlayerAssignCallback(Script* script, void* argv, uint argc)
{
	SingleArgHelper* helper = (SingleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("playerassign"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		script->ExecEventAsync("playerassign", 1, argv);
	}
	return true;
}

void PlayerAssignEvent(DWORD dwUnitId)
{
	SingleArgHelper helper = {dwUnitId};
	ScriptEngine::ForEachScript(PlayerAssignCallback, &helper, 1);
}

bool __fastcall MouseClickCallback(Script* script, void* argv, uint argc)
{
	TripleArgHelper* helper = (TripleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("mouseclick"))
	{
		AutoRoot** argv = new AutoRoot*[3];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		argv[1] = new AutoRoot(INT_TO_JSVAL(helper->arg2));
		argv[2] = new AutoRoot(INT_TO_JSVAL(helper->arg3));
		script->ExecEventAsync("mouseclick", 3, argv);
	}
	return true;
}

void MouseClickEvent(int button, POINT pt, bool bUp)
{
	TripleArgHelper helper = {button, pt.x, pt.y};
	ScriptEngine::ForEachScript(MouseClickCallback, &helper, 1);
}

bool __fastcall MouseMoveCallback(Script* script, void* argv, uint argc)
{
	DoubleArgHelper* helper = (DoubleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("mousemove"))
	{
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		argv[1] = new AutoRoot(INT_TO_JSVAL(helper->arg2));
		script->ExecEventAsync("mousemove", 2, argv);
	}
	return true;
}

void MouseMoveEvent(POINT pt)
{
	DoubleArgHelper helper = {pt.x, pt.y};
	ScriptEngine::ForEachScript(MouseMoveCallback, &helper, 1);
}

bool __fastcall BCastEventCallback(Script* script, void* argv, uint argc)
{
	BCastEventHelper* helper = (BCastEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("scriptmsg"))
	{
		AutoRoot** args = new AutoRoot*[helper->argc];
		for(uintN i = 0; i < helper->argc; i++)
			args[i] = new AutoRoot(helper->argv[i]);
		script->ExecEventAsync("scriptmsg", helper->argc, args);
	}
	return true;
}

void ScriptBroadcastEvent(uintN argc, jsval* args)
{
	BCastEventHelper helper = {args, argc};
	ScriptEngine::ForEachScript(BCastEventCallback, &helper, 1);
}

bool __fastcall GoldDropCallback(Script* script, void* argv, uint argc)
{
	DoubleArgHelper* helper = (DoubleArgHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("golddrop"))
	{
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->arg1));
		argv[1] = new AutoRoot(INT_TO_JSVAL(helper->arg2));
		script->ExecEventAsync("golddrop", 2, argv);
	}
	return true;
}

void GoldDropEvent(DWORD GID, BYTE Mode)
{
	DoubleArgHelper helper = {GID, Mode};
	ScriptEngine::ForEachScript(GoldDropCallback, &helper, 1);
}

bool __fastcall ChatEventCallback(Script* script, void* argv, uint argc)
{
	ChatEventHelper* helper = (ChatEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered(helper->event))
	{
		JSContext* cx = script->GetContext();
		AutoRoot** argv = new AutoRoot*[2];
		JS_SetContextThread(cx);
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->nick)));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->msg)));
		//JS_ClearContextThread(cx);
		script->ExecEventAsync(helper->event, 2, argv);
	}
	return true;
}

void ChatEvent(char* lpszNick, char* lpszMsg)
{
	ChatEventHelper helper = {"chatmsg", lpszNick, lpszMsg};
	ScriptEngine::ForEachScript(ChatEventCallback, &helper, 1);
}

void WhisperEvent(char* lpszNick, char* lpszMsg)
{
	ChatEventHelper helper = {"whispermsg", lpszNick, lpszMsg};
	ScriptEngine::ForEachScript(ChatEventCallback, &helper, 1);
}

bool __fastcall CopyDataCallback(Script* script, void* argv, uint argc)
{
	CopyDataHelper* helper = (CopyDataHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("copydata"))
	{
		JSContext* cx = script->GetContext();
		AutoRoot** argv = new AutoRoot*[2];
		JS_SetContextThread(cx);
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->mode));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->msg)));
		//JS_ClearContextThread(cx);
		script->ExecEventAsync("copydata", 2, argv);
	}
	return true;
}

void CopyDataEvent(DWORD dwMode, char* lpszMsg)
{
	CopyDataHelper helper = {dwMode, lpszMsg};
	ScriptEngine::ForEachScript(CopyDataCallback, &helper, 1);
}

bool __fastcall GameEventCallback(Script* script, void* argv, uint argc)
{
	if(script->IsRunning() && script->IsListenerRegistered("gamemsg"))
	{
		AutoRoot** argv = new AutoRoot*[1];
		JSContext* cx = script->GetContext();
		JS_SetContextThread(cx);
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char*)argv)));
		//JS_ClearContextThread(cx);
		script->ExecEventAsync("gamemsg", 1, argv);
	}
	return true;
}

void GameMsgEvent(char* lpszMsg)
{
	ScriptEngine::ForEachScript(GameEventCallback, lpszMsg, 1);
}

bool __fastcall ItemEventCallback(Script* script, void* argv, uint argc)
{
	ItemEventHelper* helper = (ItemEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("itemaction"))
	{
		AutoRoot** argv = new AutoRoot*[4];
		JSContext* cx = script->GetContext();
		JS_SetContextThread(cx);
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->id));
		argv[1] = new AutoRoot(INT_TO_JSVAL(helper->mode));
		argv[2] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->code)));
		argv[3] = new AutoRoot(BOOLEAN_TO_JSVAL(helper->global));
		//JS_ClearContextThread(cx);
		script->ExecEventAsync("itemaction", 4, argv);
	}
	return true;
}

void ItemActionEvent(DWORD GID, char* Code, BYTE Mode, bool Global)
{
	ItemEventHelper helper = {GID, Code, Mode, Global};
	ScriptEngine::ForEachScript(ItemEventCallback, &helper, 1);
}

bool __fastcall GameActionEventCallback(Script* script, void* argv, uint argc)
{
	GameActionEventHelper* helper = (GameActionEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("gameevent"))
	{
		AutoRoot** argv = new AutoRoot*[4];
		JSContext* cx = script->GetContext();
		JS_SetContextThread(cx);
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->mode));
		argv[1] = new AutoRoot(INT_TO_JSVAL(helper->param));
		argv[2] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->name1)));
		argv[3] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->name2)));
		//JS_ClearContextThread(cx);
		script->ExecEventAsync("gameevent", 4, argv);
	}
	return true;
}

void GameActionEvent(BYTE mode, DWORD param, char* name1, char* name2)
{
	GameActionEventHelper helper = {mode, param, name1, name2};
	ScriptEngine::ForEachScript(GameActionEventCallback, &helper, 1);
}

