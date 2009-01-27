#include "D2BS.h"

PatchHook pHooks[] = {
//	{PatchJmp,	GetDllOffset("D2GFX.dll", 0x8920+5),	(DWORD)GameMinimize_Interception, 5},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x869B5),	(DWORD)GameInput_Interception, 5},
	{PatchJmp,	GetDllOffset("D2Client.dll", 0x90524),	(DWORD)GameDraw_STUB, 6},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x65111),	(DWORD)GamePacketReceived_STUB, 5},
	{PatchCall,	GetDllOffset("D2CLIENT.dll", 0x5CEF4),	(DWORD)myGetSelectedUnit, 5},
	{PatchCall,	GetDllOffset("D2Win.dll", 0xD848),		(DWORD)GameDrawOOG, 5},
	{PatchCall,	GetDllOffset("D2MULTI.dll", 0xFC3C),	(DWORD)Whisper_Intercept, 7},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x5B050),	(DWORD)GameAttack_STUB,5},
	{PatchCall,	GetDllOffset("D2CMP.dll", 0x10DA5),		(DWORD)GameCrashFix_STUB, 10},
	{PatchJmp,	GetDllOffset("D2CLIENT.dll", 0x66487),	(DWORD)PlayerAssignment_STUB, 5},
};