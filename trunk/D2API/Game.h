#pragma once
#include <windows.h>
#include "D2Ptrs.h"

extern BOOL g_DontCatchNextMsg;
extern BOOL g_ClickAction;
extern DWORD g_SelectedUnitId;
extern DWORD g_SelectedUnitType;

void SendGold(int nGold, int nMode);
void Say(const char* szMessage, ...);
bool ClickMap(DWORD dwClickType, WORD wX = 0xFFFF, WORD wY = 0xFFFF, BOOL bShift = FALSE, UnitAny* pUnit = NULL);
