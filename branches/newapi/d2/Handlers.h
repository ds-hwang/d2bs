#pragma once

#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#include <windows.h>

#include "Structs.h"

DWORD __fastcall Input_Handler(wchar_t* wMsg);
BOOL __fastcall PacketReceived_Handler(BYTE* pPacket, DWORD dwSize);
void InternalDraw_Handler(void);
void ExternalDraw_Handler(void);
void __fastcall Whisper_Handler(char* szAcc, char* szText);
DWORD __fastcall Attack_Handler(AttackStruct* pAttack);
void __fastcall PlayerAssignment_Handler(UnitAny* pPlayer);

#endif
