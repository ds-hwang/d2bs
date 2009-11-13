#pragma once

#include <windows.h>

void ChatEvent(CHAR* lpszNick, CHAR* lpszMsg);
void LifeEvent(DWORD dwLife);
void ManaEvent(DWORD dwMana);
void CopyDataEvent(DWORD dwMode, CHAR* lpszMsg);
void GameMsgEvent(CHAR* lpszMsg);
void WhisperEvent(CHAR* lpszNick, CHAR* lpszMsg);
void KeyDownUpEvent(WPARAM bByte, BYTE bUp);
void PlayerAssignEvent(DWORD dwUnitId);
void MouseClickEvent(int button, POINT pt, bool bUp);
void MouseMoveEvent(POINT pt);
void ScriptBroadcastEvent(uintN argc, jsval* argv);
void GoldDropEvent(DWORD GID, WORD itemX, WORD itemY, WORD Mode);
void ItemDropEvent(DWORD GID, CHAR* Code, WORD itemX, WORD itemY, WORD Mode);
void DrawEvent(void);
void DrawOOGEvent(void);
