#pragma once

VOID	Print(const char * szFormat, ...);
VOID	Say(CHAR* szMessage, ...);
VOID	__fastcall Say_ASM(DWORD dwPtr);
BOOL	ClickMap(DWORD dwClickType, WORD wX, WORD wY, BOOL bShift, UnitAny* pUnit);
