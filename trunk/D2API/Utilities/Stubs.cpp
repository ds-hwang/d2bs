#include <windows.h>
#include "D2Ptrs.h"

DWORD __declspec(naked) __fastcall D2CLIENT_InitAutomapLayer_STUB(DWORD nLayerNo)
{
	__asm 
	{
		push eax;
		mov eax, ecx;
		call D2CLIENT_InitAutomapLayer_I;
		pop eax;
		ret;
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_GetUnitName_STUB(DWORD UnitAny)
{
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_GetUnitName_I
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_GetUIVar_STUB(DWORD varno)
{
	__asm 
	{
		mov eax, ecx;
		jmp D2CLIENT_GetUiVar_I;
	}
}

void __declspec(naked) __fastcall D2CLIENT_SetSelectedUnit_STUB(DWORD UnitAny)
{
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_SetSelectedUnit_I
	}
}
DWORD __declspec(naked) __fastcall D2CLIENT_LoadUIImage_ASM(char* Path) 
{
	__asm
	{
		mov eax, ecx
		push 0
		call D2CLIENT_LoadUIImage_I
		retn
	}
}

void __declspec(naked) __fastcall D2CLIENT_Interact_ASM(DWORD Struct)
{
	__asm
	{
		mov esi, ecx
		jmp D2CLIENT_Interact_I
	}
}

DWORD __declspec(naked) __fastcall D2CLIENT_ClickParty_ASM(DWORD RosterUnit, DWORD Mode)
{
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_ClickParty_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ShopAction_ASM(DWORD pItem, DWORD pNpc, DWORD pNPC, DWORD _1, DWORD pTable2 /* Could be also the ItemCost?*/, DWORD dwMode, DWORD _2, DWORD _3)
{
	__asm
	{
		jmp D2CLIENT_ShopAction_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ClickBelt(DWORD x, DWORD y, Inventory* pInventoryData)
{
	__asm
	{
		mov eax, edx
		jmp D2CLIENT_ClickBelt_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ClickItemRight_ASM(DWORD x, DWORD y, DWORD Location, DWORD Player, DWORD pUnitInventory)
{
	__asm
	{
		// ecx = y, edx = x - Blizzard is weird :D
		mov eax, ecx
		mov ecx, edx
		mov edx, eax

		pop eax
		mov ebx,eax
		pop eax
		push ebx
		jmp D2CLIENT_ClickItemRight_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_ClickBeltRight_ASM(DWORD pInventory, DWORD pPlayer, DWORD HoldShift, DWORD dwPotPos)
{
	__asm
	{
		pop eax
		mov ebx,eax
		pop eax
		push ebx
		jmp D2CLIENT_ClickBeltRight_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_GetItemDesc_ASM(DWORD pUnit, wchar_t* pBuffer)
{
	__asm 
	{
		push edi
		mov edi, edx
		push NULL
		push 1		// TRUE = New lines, FALSE = Comma between each stat value
		push ecx
		call D2CLIENT_GetItemDesc_I
		pop edi
		retn
	}
}

void __declspec(naked) __fastcall D2COMMON_DisplayOverheadMsg_ASM(DWORD pUnit)
{
	__asm
	{
		lea esi, [ecx+0xA4]
		mov eax, [ecx+0xA4]

		push eax
		push 0
		call D2COMMON_DisplayOverheadMsg_I

		retn
	}
}

void __declspec(naked) __fastcall D2CLIENT_MercItemAction_ASM(DWORD bPacketType, DWORD dwSlot)
{
	__asm 
	{
		mov eax, ecx
		mov ecx, edx
		jmp D2CLIENT_MercItemAction_I
	}
}

void __declspec(naked) __fastcall D2CLIENT_PlaySound(DWORD dwSoundId)
{
	__asm
	{
		mov ebx, ecx
		push NULL
		push NULL
		push NULL
		mov eax, p_D2CLIENT_PlayerUnit
		mov eax, [eax]
		mov ecx, eax
		mov edx, ebx
		call D2CLIENT_PlaySound_I
		retn
	}
}

__declspec(naked) void __stdcall D2CLIENT_TakeWaypoint(DWORD dwWaypointId, DWORD dwArea)
{
	__asm
	{
		push ebp
		mov ebp, esp
		sub esp, 0x20
		push ebx
		push esi
		push edi
		and dword ptr ss:[ebp-0x20],0
		push 0
		call _TakeWaypoint
		jmp _Exit

_TakeWaypoint:
		push ebp
		push esi
		push edi
		push ebx
		XOR edi, edi
		mov ebx, 1
		mov ecx, dword ptr ss:[ebp+8]
		mov edx, dword ptr ss:[ebp+0xC]
		lea ebp, dword ptr ss:[ebp-0x20]
		jmp [D2CLIENT_TakeWaypoint_I]


_Exit:
		pop edi
		pop esi
		pop ebx
		leave
		retn 8
	}
}
DWORD __declspec(naked) __fastcall D2CLIENT_TestPvpFlag_STUB(DWORD planum1, DWORD planum2, DWORD flagmask)
{
	__asm 
	{
		push esi;
		push [esp+8];
		mov esi, edx;
		mov edx, ecx;
		call D2CLIENT_TestPvpFlag_I;
		pop esi;
		ret 4;
	}
}

void __declspec(naked) __fastcall D2GFX_DrawRectFrame_STUB(RECT* rect)
{
	__asm
	{
		mov eax, ecx;
		jmp D2CLIENT_DrawRectFrame;
	}
}

DWORD __cdecl D2CLIENT_GetMinionCount(UnitAny* pUnit, DWORD dwType)
{
	DWORD dwResult;

	__asm
	{
		push eax
		push esi
		mov eax, pUnit
		mov esi, dwType
		call D2CLIENT_GetMinionCount_I
		mov [dwResult], eax
		pop esi
		pop eax
	}

	return dwResult;
}

void __declspec(naked) __fastcall D2CLIENT_HostilePartyUnit(RosterUnit* pUnit, DWORD dwButton)
{
	__asm
	{
		mov eax, edx
		jmp [D2CLIENT_ClickParty_II]
	}
}
