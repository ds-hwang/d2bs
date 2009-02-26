#pragma once

#ifndef __D2UTILITIES_H__
#define __D2UTILITIES_H__

#include "Structs.h"

bool GameReady(void);
void D2BSCleanup(void);
void D2Print(const char* format, int color, ...);
UnitAny* FindUnit(DWORD id, DWORD type);
RosterUnit* FindPlayerRoster(DWORD id);

#endif

