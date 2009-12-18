#include "D2Handlers.h"
#include "D2Ptrs.h"
#include "D2BS.h"

VOID __declspec(naked) GamePacketReceived_STUB()
{
	__asm
	{
		pop ebp;
		pushad;

		call GamePacketReceived;
		test eax, eax;

		popad;
		jnz OldCode;

		mov edx, 0;

OldCode:
		call D2NET_ReceivePacket_I;

		push ebp;
		ret;
	}
}

VOID __declspec(naked) GameDraw_STUB()
{
	__asm
	{
		call GameDraw;

		POP ESI
		POP EBX
		POP ECX
		RETN 4
	}
}

VOID __declspec(naked) GameInput_Interception()
{
	__asm {
	pushad
		mov ecx, ebx
		call GameInput
		cmp eax, -1
	popad
	je BlockIt
	call D2CLIENT_InputCall_I
	ret

BlockIt:

	xor eax,eax
	ret
	}
}

BOOL GameMinimize_Interception()
{
	if(D2CLIENT_GetPlayerUnit() && GetForegroundWindow() != D2GFX_GetHwnd())
		return 1;

	return 0;
}


UnitAny* myGetSelectedUnit(VOID)
{
	if(Vars.bClickAction)
	{
		if(Vars.dwSelectedUnitId)
		{
			UnitAny* pUnit = D2CLIENT_FindUnit(Vars.dwSelectedUnitId, Vars.dwSelectedUnitType);

			return pUnit;
		}

		return NULL;
	}

	return D2CLIENT_GetSelectedUnit();
}

VOID __declspec(naked) Whisper_Intercept()
{
	__asm
	{
		MOV EBP,DWORD PTR SS:[ESP+0x1FC+4]
		pushad
		mov ecx, edx
		mov edx, ebx
		call WhisperHandler
		popad
		//jmp D2MULTI_WhisperIntercept_Jump
		retn
	}
}

VOID __declspec(naked) GameAttack_STUB()
{
	__asm 
	{
		push ecx
		mov ecx, [esp+0xC]
		call GameAttack
		pop ecx

		cmp eax, -1
		je OldCode

		call D2CLIENT_GetSelectedUnit
		
		cmp eax, 0
		je OldCode

		mov [esp+0x0C], 1

OldCode:
		mov eax, [D2CLIENT_Attack_I]
		mov eax, [eax]
		retn
	}
}

VOID __declspec(naked) PlayerAssignment_STUB()
{
	__asm
	{
		FNOP
		CALL D2CLIENT_AssignPlayer_I
		MOV ECX, EAX
		CALL GamePlayerAssignment
		RETN
	}
}

VOID __declspec(naked) GameCrashFix_STUB()
{
	__asm
	{
		CMP ECX, 0
		JE Skip
		MOV DWORD PTR DS:[ECX+0x10],EDX
Skip:
		MOV DWORD PTR DS:[EAX+0xC],0
		RETN
	}
}

VOID GameDraw_Interception(VOID)
{
	GameDrawOOG();
}
