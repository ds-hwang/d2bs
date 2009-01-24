#pragma once

VOID ChatEvent(CHAR* lpszNick, CHAR* lpszMsg);
VOID LifeEvent(DWORD dwLife, DWORD dwMana);
VOID CopyDataEvent(DWORD dwMode, CHAR* lpszMsg);
VOID ChatCmdEvent(CHAR* lpszMsg);
VOID BNCSChatEvent(CHAR* lpszNick, CHAR* lpszMsg);
VOID KeyDownUpEvent(WPARAM bByte, BYTE bUp);
VOID PlayerAssignEvent(DWORD dwUnitId);
VOID MouseMoveEvent(POINT pt);
VOID ScriptBroadcastEvent(uintN argc, jsval* argv);
