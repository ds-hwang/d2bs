#pragma once

#include "Offset.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"

PatchHook Patches[] = {
	{PatchCall,	GetDllOffset("D2Client.dll", 0x70B75),	(DWORD)GameInput_Intercept,				5},
	{PatchJmp,	GetDllOffset("D2Client.dll", 0xC3DB4),	(DWORD)GameDraw_Intercept,				6},
	{PatchCall,	GetDllOffset("D2Client.dll", 0xACE61),	(DWORD)GamePacketReceived_Intercept,	5},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x1BF94),	(DWORD)GetSelectedUnit_Intercept,		5},
	{PatchJmp,	GetDllOffset("D2Client.dll", 0xADF37),	(DWORD)PlayerAssignment_Intercept,		5},
	{PatchBytes,GetDllOffset("D2Client.dll", 0x161B0),	(DWORD)0xc3,							1},
	{PatchCall, GetDllOffset("D2Client.dll", 0xC14F7),	(DWORD)GameActChange_Intercept,			5},
	{PatchJmp,  GetDllOffset("D2Client.dll", 0xC19A4),	(DWORD)GameActChange2_Intercept,		5},
	{PatchCall, GetDllOffset("D2Client.dll", 0x4515D),	(DWORD)GameLeave_Intercept,				5},
	{PatchCall,	GetDllOffset("D2Client.dll", 0x1A060),	(DWORD)GameAttack_Intercept,			5},
//	{PatchCall,	GetDllOffset("D2Client.dll", 0xA7364),	(DWORD)AddUnit_Intercept,				5},
//	{PatchCall,	GetDllOffset("D2Client.dll", 0xA6F25),	(DWORD)RemoveUnit_Intercept,			9},

	{PatchCall,	GetDllOffset("D2Multi.dll", 0x1000C),	(DWORD)Whisper_Intercept,				7},
	{PatchCall, GetDllOffset("D2Multi.dll", 0xD753),	(DWORD)ChannelInput_Intercept,			5},
	{PatchCall,	GetDllOffset("D2Multi.dll", 0x10781),	(DWORD)ChannelWhisper_Intercept,		5},
	{PatchJmp,	GetDllOffset("D2Multi.dll", 0x108A0),	(DWORD)ChannelChat_Intercept,			6},
	{PatchJmp,	GetDllOffset("D2Multi.dll", 0x107A0),	(DWORD)ChannelEmote_Intercept,			6},
	
	{PatchCall,	GetDllOffset("D2Win.dll", 0x18918),		(DWORD)GameDrawOOG_Intercept,			5},

	{PatchCall,	GetDllOffset("D2CMP.dll", 0x17385),		(DWORD)GameCrashFix_Intercept,			10},
};