#include "ScriptEngine.h"

void LifeEvent(DWORD dwLife)
{
	ScriptEngine::ExecEventAsync("melife", "u", (uint32)dwLife);
}

void ManaEvent(DWORD dwMana)
{
	ScriptEngine::ExecEventAsync("memana", "u", (uint32)dwMana);
}

void KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	ScriptEngine::ExecEventAsync((bUp ? "keyup" : "keydown"), "u", (uint32)key);
}

void PlayerAssignEvent(DWORD dwUnitId)
{
	ScriptEngine::ExecEventAsync("playerassign", "u", (uint32)dwUnitId);
}

void MouseClickEvent(int button, POINT pt, bool bUp)
{
	ScriptEngine::ExecEventAsync("mouseclick", "ubuu", (uint32)button, (JSBool)bUp, (uint32)pt.x, (uint32)pt.y);
}

void MouseMoveEvent(POINT pt)
{
	ScriptEngine::ExecEventAsync("mousemove", "uu", (uint32)pt.x, (uint32)pt.y);
}


void ScriptBroadcastEvent(uintN argc, jsval* args)
{
	char* format = new char[argc+1];
	for(uintN i = 0; i < argc; i++)
		format[i] = 'v';
	format[argc] = '\0';
	ScriptEngine::ExecEventAsync("scriptmsg", format, args);
	delete[] format;
}

void GoldDropEvent(DWORD GID, BYTE Mode)
{
	ScriptEngine::ExecEventAsync("golddrop", "uu", (uint32)GID, (uint32)Mode);
}

void ChatEvent(char* lpszNick, char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("chatmsg", "ss", lpszNick, lpszMsg);
}

void WhisperEvent(char* lpszNick, char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("whispermsg", "ss", lpszNick, lpszMsg);
}

void CopyDataEvent(DWORD dwMode, char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("copydata", "is", (int32)dwMode, lpszMsg);
}

void GameMsgEvent(char* lpszMsg)
{
	ScriptEngine::ExecEventAsync("gamemsg", "s", lpszMsg);
}

void ItemActionEvent(DWORD gid, char* code, BYTE mode, bool global)
{
	ScriptEngine::ExecEventAsync("itemaction", "usub", (uint32)gid, code, (uint32)mode, (JSBool)global);
}

void GameActionEvent(BYTE mode, DWORD param, char* name1, char* name2)
{
	ScriptEngine::ExecEventAsync("gameevent", "uuss", (uint32)mode, (uint32)param, name1, name2);
}
