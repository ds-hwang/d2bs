#include "ScriptEngine.h"

#include "debugnew/debug_new.h"

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
	WORD x;
	WORD y;
	WORD mode;
};

bool __fastcall ExecEventHelper(Script* script, void* argv, uint argc)
{
	EventHelper* helper = (EventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered(helper->evtName))
		script->ExecEventAsync(helper->evtName, helper->argc, helper->argv);
	else
	{
		for(int i = 0; i < helper->argc; i++)
			delete helper->argv[i];
	}
	return true;
}

void LifeEvent(DWORD dwLife)
{
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(INT_TO_JSVAL(dwLife));
	EventHelper helper = {"melife", argv, 1};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void ManaEvent(DWORD dwMana)
{
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(INT_TO_JSVAL(dwMana));
	EventHelper helper = {"memana", argv, 1};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(INT_TO_JSVAL(key));
	EventHelper helper = {(bUp ? "keyup" : "keydown"), argv, 1};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void PlayerAssignEvent(DWORD dwUnitId)
{
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(INT_TO_JSVAL(dwUnitId));
	EventHelper helper = {"playerassign", argv, 1};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void MouseClickEvent(int button, POINT pt, bool bUp)
{
	AutoRoot** argv = new AutoRoot*[3];
	argv[0] = new AutoRoot(INT_TO_JSVAL(button));
	argv[1] = new AutoRoot(INT_TO_JSVAL(pt.x));
	argv[2] = new AutoRoot(INT_TO_JSVAL(pt.y));
	EventHelper helper = {(bUp ? "mouseup" : "mousedown"), argv, 3};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void MouseMoveEvent(POINT pt)
{
	AutoRoot** argv = new AutoRoot*[2];
	argv[0] = new AutoRoot(INT_TO_JSVAL(pt.x));
	argv[1] = new AutoRoot(INT_TO_JSVAL(pt.y));
	EventHelper helper = {"mousemove", argv, 2};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void ScriptBroadcastEvent(uintN argc, jsval* args)
{
	AutoRoot** argv = new AutoRoot*[argc];
	for(uintN i = 0; i < argc; i++)
		argv[i] = new AutoRoot(args[i]);
	EventHelper helper = {"scriptmsg", argv, argc};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

void GoldDropEvent(DWORD GID, WORD itemX, WORD itemY, WORD Mode)
{
	AutoRoot** argv = new AutoRoot*[4];
	argv[0] = new AutoRoot(INT_TO_JSVAL(GID));
	argv[1] = new AutoRoot(INT_TO_JSVAL(itemX));
	argv[2] = new AutoRoot(INT_TO_JSVAL(itemY));
	argv[3] = new AutoRoot(INT_TO_JSVAL(Mode));
	EventHelper helper = {"golddrop", argv, 4};
	ScriptEngine::ForEachScript(ExecEventHelper, &helper, 1);
}

bool __fastcall ChatEventCallback(Script* script, void* argv, uint argc)
{
	ChatEventHelper* helper = (ChatEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered(helper->event))
	{
		JSContext* cx = script->GetContext();
		AutoRoot** argv = new AutoRoot*[2];
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->nick)));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->msg)));
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
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->mode));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(cx, helper->msg)));
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
		argv[0] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(script->GetContext(), (char*)argv)));
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
	if(script->IsRunning() && script->IsListenerRegistered("itemdrop"))
	{
		AutoRoot** argv = new AutoRoot*[5];
		argv[0] = new AutoRoot(INT_TO_JSVAL(helper->id));
		argv[1] = new AutoRoot(STRING_TO_JSVAL(JS_NewStringCopyZ(script->GetContext(), helper->code)));
		argv[2] = new AutoRoot(INT_TO_JSVAL(helper->x));
		argv[3] = new AutoRoot(INT_TO_JSVAL(helper->y));
		argv[4] = new AutoRoot(INT_TO_JSVAL(helper->mode));
		script->ExecEventAsync("itemdrop", 5, argv);
	}
	return true;
}

void ItemDropEvent(DWORD GID, char* Code, WORD itemX, WORD itemY, WORD Mode)
{
	ItemEventHelper helper = {GID, Code, itemX, itemY, Mode};
	ScriptEngine::ForEachScript(ItemEventCallback, &helper, 1);
}
