#pragma once

DWORD WINAPI D2Thread(LPVOID lpParam);
DWORD __fastcall GameInput(wchar_t* wMsg);
LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam);
VOID GameDraw(VOID);
DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize);
VOID __fastcall GameResetFrames(UnitAny* pOwner);
VOID GameDrawOOG(VOID);
VOID __fastcall WhisperHandler(CHAR* szAcc, CHAR* szText);
DWORD __fastcall GameAttack(AttackStruct* pAttack);
VOID __fastcall GamePlayerAssignment(UnitAny* pPlayer);
