#pragma once

VOID ChatEvent(CHAR* lpszNick, CHAR* lpszMsg);
VOID LifeEvent(DWORD dwLife);
VOID ManaEvent(DWORD dwMana);
VOID CopyDataEvent(DWORD dwMode, CHAR* lpszMsg);
VOID GameMsgEvent(CHAR* lpszMsg);
VOID WhisperEvent(CHAR* lpszNick, CHAR* lpszMsg);
VOID KeyDownUpEvent(WPARAM bByte, BYTE bUp);
VOID PlayerAssignEvent(DWORD dwUnitId);
VOID MouseClickEvent(int button, POINT pt, bool bUp);
VOID MouseMoveEvent(POINT pt);
VOID ScriptBroadcastEvent(uintN argc, jsval* argv);
VOID ItemDropEvent(DWORD GID,CHAR* Code,WORD itemX,WORD itemY,WORD Mode);
