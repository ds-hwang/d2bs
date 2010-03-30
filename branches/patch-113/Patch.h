#pragma once

#include "Offset.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"

PatchHook pHooks[] = {
	{PatchCall,	GetDllOffset("D2Client.dll", 0x70B75),	(DWORD)GameInput_Interception,	5},
	{PatchJmp,	GetDllOffset("D2Client.dll", 0xC3DB4),	(DWORD)GameDraw_STUB,			6},
	{PatchCall,	GetDllOffset("D2Client.dll", 0xACE61),	(DWORD)GamePacketReceived_STUB,	5},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x1BF94),	(DWORD)myGetSelectedUnit,		5},
	{PatchCall,	GetDllOffset("D2Win.dll", 0x18918),		(DWORD)GameDraw_Interception,	5},
	{PatchCall,	GetDllOffset("D2Multi.dll", 0x1000C),	(DWORD)Whisper_Intercept,		7},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x1A060),	(DWORD)GameAttack_STUB,			5},
	{PatchCall,	GetDllOffset("D2CMP.dll", 0x17385),		(DWORD)GameCrashFix_STUB,		10},
	{PatchJmp,	GetDllOffset("D2Client.dll", 0xADF37),	(DWORD)PlayerAssignment_STUB,	5},
	{PatchBytes,GetDllOffset("D2Client.dll", 0x161B0),	(DWORD)0xc3,					1},
	{PatchCall, GetDllOffset("D2Client.dll", 0xC14F7),	(DWORD)GameActChange_STUB,		5},
	{PatchJmp,  GetDllOffset("D2Client.dll", 0xC19A4),	(DWORD)GameActChange2_STUB,		5},
	{PatchCall, GetDllOffset("D2Client.dll", 0x4515D),	(DWORD)GameLeave_STUB,			5},
};