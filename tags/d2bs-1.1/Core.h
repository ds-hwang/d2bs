#pragma once

#include <windows.h>

#include "D2Structs.h"

void	Print(const char * szFormat, ...);
void	Say(const char* szMessage, ...);
//void	__fastcall Say_ASM(DWORD dwPtr);
bool	ClickMap(DWORD dwClickType, WORD wX, WORD wY, BOOL bShift, UnitAny* pUnit);
