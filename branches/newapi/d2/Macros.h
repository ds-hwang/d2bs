#pragma once

#ifndef __MACROS_H__
#define __MACROS_H__

#include <windows.h>

#include "Offsets.h"

//D2Gfx = 6FA80000  D2Win = 6F8E0000  D2Client = 6FAB0000  D2Net = 0x6FBF0000
enum Dll {D2Client,D2Common,D2Gfx,D2Lang,D2Win,D2Net,D2Game,D2Launch,Fog,BNClient,Storm,D2Cmp,D2Multi};

#ifdef _CREATE_DEFINITIONS

#define FUNCTION(mod, addr, ret, call, name, args) \
	__declspec(naked) ret call name args { \
		static DWORD f##name = NULL; \
		if(f##name == NULL) { \
			__asm { pushad } __asm { pushfd } \
			f##name = GetDllAddress((int)mod, addr); \
			 __asm { popfd }__asm { popad } \
		} \
		__asm { jmp f##name } \
	}

#define VARIABLE(mod, type, name, addr) \
	type __fastcall Get##mod##name##(void) { \
		return (type)*(DWORD*)GetDllAddress((int)mod, addr); \
	} \
	type __fastcall mod##name##Addr(void) { \
		return (type)(DWORD*)GetDllAddress((int)mod, addr); \
	}

#define ADDRESS(mod, name, addr) \
	DWORD __fastcall Get##mod##name##_ASM(void) { \
		return (DWORD)GetDllAddress((int)mod, addr); \
	}

#else

#define FUNCTION(mod, off, ret, call, name, args) \
	ret call name args;

#define VARIABLE(mod, type, name, off) \
	type __fastcall Get##mod##name##(void); \
	type __fastcall mod##name##Addr(void);

#define ADDRESS(mod, name, addr) \
	DWORD __fastcall Get##mod##name##_ASM(void);

#endif

#endif

