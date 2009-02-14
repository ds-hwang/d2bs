#include <windows.h>

#include "Handlers.h"
#include "Functions.h"
#include "Structs.h"
#include "Constants.h"

#include "Threads.h"

#include "prthread.h"

#include "debug_new.h"

DWORD __fastcall Input_Handler(wchar_t* wMsg)
{
	return 0;
}

BOOL __fastcall PacketReceived_Handler(BYTE* pPacket, DWORD dwSize)
{
	return TRUE;
}

void InternalDraw_Handler(void)
{
}

void ExternalDraw_Handler(void)
{
	DrawSprites();
	static PRThread* D2BSThread = NULL;
	if(D2BSThread == NULL)
	{
		D2BSThread = PR_CreateThread(PR_USER_THREAD, MainThread, 0, PR_PRIORITY_NORMAL, PR_GLOBAL_THREAD, PR_JOINABLE_THREAD, 0);
	}
	InternalDraw_Handler();
}

void __fastcall Whisper_Handler(char* szAcc, char* szText)
{
}

DWORD __fastcall Attack_Handler(AttackStruct* pAttack)
{
	if(!pAttack)
		return -1;

	if(!pAttack->lpTargetUnit)
		return -1;

	if(pAttack->lpTargetUnit->dwType != UNIT_MONSTER)
		return -1;

	if(pAttack->dwAttackType == ATTACKTYPE_UNITLEFT)
		pAttack->dwAttackType = ATTACKTYPE_SHIFTLEFT;

	if(pAttack->dwAttackType == ATTACKTYPE_RIGHT)
		pAttack->dwAttackType = ATTACKTYPE_SHIFTRIGHT;

	return NULL;
}

void __fastcall PlayerAssignment_Handler(UnitAny* pPlayer)
{
}
