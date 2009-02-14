#pragma once

#ifndef __PATCHTABLE_H__
#define __PATCHTABLE_H__

#include "Intercepts.h"

enum PatchType { NoPatch, JumpPatch, CallPatch };

struct Offset {
	const char* DllName;
	int Address;
	DWORD FullAddress;
};

struct Patch {
	PatchType Type;
	Offset Place;
	DWORD NewValue;
	size_t Size;
	BYTE* OldValue;
};

static Patch Patches[] = {
	{CallPatch,		{"D2Client.dll",	0x869B5},		(DWORD)&GameInput_Intercept,			5},
	{JumpPatch,		{"D2Client.dll",	0x90524},		(DWORD)&GameInternalDraw_Intercept,		6},
	{CallPatch,		{"D2Client.dll",	0x65111},		(DWORD)&GamePacketReceived_Intercept,	5},
	{CallPatch,		{"D2Client.dll",	0x5B050},		(DWORD)&GameAttack_Intercept,			5},
	{JumpPatch,		{"D2Client.dll",	0x66487},		(DWORD)&GamePlayerAssign_Intercept,		5},
	// TODO: disabled until the code is redone... need to figure out a better scheme for globals
	//{CallPatch,		{"D2Client.dll",	0x5CEF4},		(DWORD)&GameGetSelectedUnit_Intercept,	5},
	// TODO: this patch crashes...
	//{JumpPatch,		{"D2Gfx.dll",		0x8925},		(DWORD)GameMinimize_Intercept,			5},
	{CallPatch,		{"D2Win.dll",		0xD848},		(DWORD)&GameExternalDraw_Intercept,		5},
	{CallPatch,		{"D2Multi.dll",		0xFC3C},		(DWORD)&GameWhisper_Intercept,			7},
	{CallPatch,		{"D2Cmp.dll",		0x10DA5},		(DWORD)&GameCrashFix_Intercept,			10},
	{NoPatch}
};

#endif
