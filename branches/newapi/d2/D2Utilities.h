#pragma once

#ifndef __D2UTILITIES_H__
#define __D2UTILITIES_H__

#include "Structs.h"

bool GameReady(void);
UnitAny* FindUnit(DWORD id, DWORD type);
void D2Print(char* szText, ...);

#endif
