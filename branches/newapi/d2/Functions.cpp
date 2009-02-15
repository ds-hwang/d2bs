#define _CREATE_DEFINITIONS

#include "Macros.h"
#include "Functions.h"
#include "Pointers.h"

#include "debug_new.h"

__declspec(naked) DWORD __fastcall GetUIVar(DWORD varno)
{
	__asm 
	{
		mov eax, ecx;
		jmp GetUiVar_I;
	}
}
