#pragma once

#ifndef __OFFSETS_H__
#define __OFFSETS_H__

#include <windows.h>
#include "Patch.h"

void InterceptLocalCode(PatchType, DWORD, DWORD, size_t);
bool ReadBytes(DWORD, void*, size_t);
bool WriteBytes(DWORD, void*, size_t);
void ApplyPatches(void);
void RemovePatches(void);
void FillDllAddress(Offset&);
DWORD GetDllAddress(int, int);
DWORD GetDllAddress(const char*, int);
DWORD GetDllOffset(HMODULE, int);
DWORD GetDllOrdinal(HMODULE, int);

#endif
