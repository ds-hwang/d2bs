#pragma once

#include <Windows.h>

#include "D2Api.hpp"

enum InstType
{
	INST_INT3	= 0xCC,
	INST_CALL	= 0xE8,
	INST_NOP	= 0x90,
	INST_JMP	= 0xE9,
	INST_RET	= 0xC3
};

enum PatchType {
	CALL,
	JMP
};

typedef struct PatchHook_t
{
	PatchType Type;
	DWORD dwDllNo;
	DWORD dwAddr;
	DWORD dwFunc;
	DWORD dwLen;
	BYTE *bOldCode;
} PatchHook;

PatchHook pHooks[] = {
	{CALL, D2CLIENT, 0xC3A9C, (DWORD)D2Client::PrintGameString, 5},
	//{CALL, D2CLIENT, 0x6175D, (DWORD)PlayerBlobIntercept, 6},
	//{CALL, D2CLIENT, 0x1A060), (DWORD)GameAttack_Intercept,	5}, 
	//{CALL,	D2CLIENT, 0x70B75,	(DWORD)GameInput_Interception,	5},
	//{JMP,	D2CLIENT, 0xC3DB4,	(DWORD)D2Client::GameDraw_I,	6},
	//{CALL,	D2WIN, 0x18918,		(DWORD)D2Win::OOGGameDrawHandler,	5},
	//{CALL,	D2CLIENT, 0xACE61, (DWORD)D2Client::GamePacketReceived_I, 5},
	//{CALL,	D2CLIENT, 0x1BF94,	(DWORD)GetSelectedUnit_Intercept, 5},
	//{CALL, D2CLIENT, 0x44230,	(DWORD)D2Client::GameLoop, 7},
	//{CALL, D2CLIENT, 0xA9A37,	(DWORD)LightingPatch, 6},
	//{JMP, D2COMMON, 0x6CC56,	(DWORD)WeatherPatch, 5}, // This doesn't work for me(eBoLA) or SE
	//{CALL, D2CLIENT, 0x66623, (DWORD)InfravisionPatch_ASM, 7},
	//{JMP, D2CLIENT, 0x626C9, (DWORD)D2Client::DrawAutomap_II, 5},
	//{CALL, D2MULTI, 0x14D29, (DWORD)NextGameNamePatch, 5},
	//{CALL, D2MULTI, 0x14A0B, (DWORD)NextGameNamePatch, 5},
	//{CALL, D2MULTI, 0x14A46, (DWORD)NextGamePassPatch, 5},
	//{CALL, D2MULTI, 0x14D64, (DWORD)NextGamePassPatch, 5},
	//{CALL, D2CLIENT, 0x92366, (DWORD)ItemNamePatch_ASM, 6},
	//{CALL, D2CLIENT, 0xA2E13, (DWORD)GameCreateMissileStub1, 6},
	//{CALL, D2CLIENT, 0xA2DA8, (DWORD)GameCreateMissileStub2, 7},
	//{CALL, D2CLIENT, 0x4363E, (DWORD)GameFailToJoin_STUB, 6},
	//{CALL, D2CLIENT, 0x442A2, (DWORD)GameShakePatch, 5},
	//{CALL, DCLIENT, 0x3CB20, (DWORD)GameMessageLoop_STUB, 6},
	//{CALL, D2CLIENT, 0xA6941, (DWORD)OutTownSelectPatch_ASM, 6},
};

class PatchManager {
private:
	PatchHook *RetrievePatchHooks(int* pBuffer);
	void PatchBytes(DWORD dwAddr, DWORD dwValue, DWORD dwLen);
	void PatchJmp(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
	void PatchCall(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
	bool InterceptLocalCode(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen);
	bool FillBytes(void *pAddr, BYTE bFill, DWORD dwLen);
	bool WriteBytes(void *pAddr, void *pData, DWORD dwLen);

public:
	PatchManager() {}
	~PatchManager() {}

	void RemovePatches();
	bool InstallPatches();
};
