#include "ScriptEngine.h"

void LifeEvent(DWORD dwLife)
{
	ScriptEngine::ExecEventAsync("melife", "u", 1, (uint32)dwLife);
}

void ManaEvent(DWORD dwMana)
{
	ScriptEngine::ExecEventAsync("memana", "u", 1, (uint32)dwMana);
}

void KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	ScriptEngine::ExecEventAsync((bUp ? "keyup" : "keydown"), "u", 1, (uint32)key);
}

void PlayerAssignEvent(DWORD dwUnitId)
{
	ScriptEngine::ExecEventAsync("playerassign", "u", 1, (uint32)dwUnitId);
}

void MouseClickEvent(int button, POINT pt, bool bUp)
{
	ScriptEngine::ExecEventAsync("mouseclick", "ubuu", 3, (uint32)button, (JSBool)bUp, (uint32)pt.x, (uint32)pt.y)
}

void MouseMoveEvent(POINT pt)
{
	ScriptEngine::ExecEventAsync("mousemove", "uu", 2, (uint32)pt.x, (uint32)pt.y);
}

bool __fastcall BCastEventCallback(Script* script, void* argv, uint argc)
{
//	BCastEventHelper* helper = (BCastEventHelper*)argv;
	if(script->IsRunning() && script->IsListenerRegistered("scriptmsg"))
	{
//		AutoRoot** args = new AutoRoot*[helper->argc];
//		for(uintN i = 0; i < helper->argc; i++)
//			args[i] = new AutoRoot(helper->argv[i]);
//		script->ExecEventAsync("scriptmsg", helper->argc, args);
	}
	return true;
}

void ScriptBroadcastEvent(uintN argc, jsval* args)
{
//	BCastEventHelper helper = {args, argc};
//	ScriptEngine::ForEachScript(BCastEventCallback, &helper, 1);
}

void GoldDropEvent(DWORD GID, BYTE Mode)
{
	ScriptEngine::ExecEventAsync("golddrop", "uu", 2, (uint32)GID, (uint32)Mode);
}

void ChatEvent(char* lpszNick, char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("chatmsg", "ss", 2, lpszNick, lpszMsg);
}

void WhisperEvent(char* lpszNick, char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("whispermsg", "ss", 2, lpszNick, lpszMsg);
}

void CopyDataEvent(DWORD dwMode, char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("copydata", "is", 2, (int32)dwMode, lpszMsg);
}

void GameMsgEvent(char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("gamemsg", "s", 1, lpszMsg);
}

void ItemActionEvent(DWORD gid, char* code, BYTE mode, bool global)
{
	ScriptEngine::ExecEventAsync("itemaction", "usub", 4, (uint32)gid, code, (uint32)mode, (JSBool)global);
}

void GameActionEvent(BYTE mode, DWORD param, char* name1, char* name2)
{
	ScriptEngine::ExecEventAsync("gameevent", "uuss", 4, (uint32)mode, (uint32)param, name1, name2);
}
