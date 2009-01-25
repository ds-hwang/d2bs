#include "D2BS.h"
#include "Script.h"

VOID ChatEvent(CHAR* lpszNick, CHAR* lpszMsg)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[2];
	argv[0] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszNick)));
	argv[1] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
	Script::ExecEventAsyncOnAll("chatmsg", 2, argv);
}

VOID BNCSChatEvent(CHAR* lpszNick, CHAR* lpszMsg)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[2];
	argv[0] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszNick)));
	argv[1] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
	Script::ExecEventAsyncOnAll("gamemsg", 2, argv);
}

VOID LifeEvent(DWORD dwLife, DWORD dwMana)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[2];
	argv[0] = new AutoRoot(cx, INT_TO_JSVAL(dwLife));
	argv[1] = new AutoRoot(cx, INT_TO_JSVAL(dwMana));
	Script::ExecEventAsyncOnAll("melife", 2, argv);
}

VOID CopyDataEvent(DWORD dwMode, CHAR* lpszMsg)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[2];
	argv[0] = new AutoRoot(cx, INT_TO_JSVAL(dwMode));
	argv[1] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
	Script::ExecEventAsyncOnAll("copydata", 2, argv);
}

VOID ChatCmdEvent(CHAR* lpszMsg)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, lpszMsg)));
	Script::ExecEventAsyncOnAll("chatcmd", 1, argv);
}

VOID KeyDownUpEvent(WPARAM key, BYTE bUp)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(cx, INT_TO_JSVAL(key));
	Script::ExecEventAsyncOnAll((bUp ? "keyup" : "keydown"), 1, argv);
}

VOID PlayerAssignEvent(DWORD dwUnitId)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[1];
	argv[0] = new AutoRoot(cx, INT_TO_JSVAL(dwUnitId));
	Script::ExecEventAsyncOnAll("playerassign", 1, argv);
}

VOID MouseClickEvent(int button, POINT pt, bool bUp)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[3];
	argv[0] = new AutoRoot(cx, INT_TO_JSVAL(button));
	argv[1] = new AutoRoot(cx, INT_TO_JSVAL(pt.x));
	argv[2] = new AutoRoot(cx, INT_TO_JSVAL(pt.y));
	Script::ExecEventAsyncOnAll((bUp ? "mouseup" : "mousedown"), 3, argv);
}

VOID MouseMoveEvent(POINT pt)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[2];
	argv[0] = new AutoRoot(cx, INT_TO_JSVAL(pt.x));
	argv[1] = new AutoRoot(cx, INT_TO_JSVAL(pt.y));
	Script::ExecEventAsyncOnAll("mousemove", 2, argv);
}

VOID ScriptBroadcastEvent(uintN argc, jsval* args)
{
	if(Script::GetCount() < 1)
		return;
	JSContext* cx = Script::GetFirstScript()->second->GetContext();
	AutoRoot** argv = new AutoRoot*[argc];
	for(uintN i = 0; i < argc; i++)
		argv[i] = new AutoRoot(cx, args[i]);
	Script::ExecEventAsyncOnAll("scriptmsg", argc, argv);
}
