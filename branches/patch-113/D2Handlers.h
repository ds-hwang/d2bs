#pragma once

#include <windows.h>
#include "D2Structs.h"

DWORD WINAPI D2Thread(LPVOID lpParam);
DWORD __fastcall GameInput(wchar_t* wMsg);
LONG WINAPI GameEventHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseMove(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK KeyPress(int code, WPARAM wParam, LPARAM lParam);
void GameDraw(void);
DWORD __fastcall GamePacketReceived(BYTE* pPacket, DWORD dwSize);
void __fastcall GameResetFrames(UnitAny* pOwner);
void GameDrawOOG(void);
void __fastcall WhisperHandler(char* szAcc, char* szText);
DWORD __fastcall GameAttack(AttackStruct* pAttack);
void __fastcall GamePlayerAssignment(UnitAny* pPlayer);
void GameLeave(void);
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
