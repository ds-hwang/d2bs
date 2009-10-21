#define _CREATE_DEFINITIONS

#include "Macros.h"
#include "Functions.h"
#include "Pointers.h"

#include "debug_new.h"

__declspec(naked) DWORD __fastcall TestPvpFlag(DWORD planum1, DWORD planum2, DWORD flagmask)
{
	static DWORD TestPvpFlag_I = NULL;

	if(!TestPvpFlag_I)
		TestPvpFlag_I = GetD2ClientTestPvpFlag_I_ASM();

	__asm 
	{
		push esi
		push [esp+8]
		mov esi, edx
		mov edx, ecx
		call TestPvpFlag_I
		pop esi
		ret 4
	}
}

__declspec(naked) DWORD __fastcall clickParty(RosterUnit* pUnit, INT Mode)
{
	static DWORD ClickParty_I = NULL;

	if(!ClickParty_I)
		ClickParty_I = GetD2ClientClickParty_I_ASM();

	__asm
	{
		mov eax, ecx
		jmp ClickParty_I
	}
}

__declspec(naked) DWORD __fastcall GetUIVar(DWORD varno)
{
	__asm 
	{
		mov eax, ecx
		jmp GetUiVar_I
	}
}

