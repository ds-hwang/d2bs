#pragma once

#ifndef __CORE_H__
#define __CORE_H__

#include <windows.h>
#include <string>
#include <list>

#include "D2Structs.h"

void Print(const char * szFormat, ...);
void Say(const char* szMessage, ...);
bool ClickMap(DWORD dwClickType, WORD wX = 0xFFFF, WORD wY = 0xFFFF, BOOL bShift = FALSE, UnitAny* pUnit = NULL);

#endif
