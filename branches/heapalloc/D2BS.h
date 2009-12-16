#ifndef __D2BS_H__
#define __D2BS_H__

#pragma once

#define XP_WIN
#define JS_THREADSAFE

#define D2BS_VERSION "1.1.1"

#include <windows.h>

#include "CollisionMap.h"
#include "ScreenHook.h"

struct Variables;

#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))

#define PRIVATE_UNIT		1
#define PRIVATE_ITEM		3

struct Private { DWORD dwPrivateType; };

struct Module
{	
	union {
		HMODULE hModule;
		DWORD dwBaseAddress;
	};
	DWORD _1;
	CHAR szPath[MAX_PATH];
};

struct Variables
{
	INT		nChickenHP;
	INT		nChickenMP;
	DWORD	dwInjectTime;
	DWORD	dwGameTime;
	BOOL	bDontCatchNextMsg;
	BOOL	bClickAction;
	BOOL	bNeedShutdown;
	BOOL	bUseGamePrint;
	BOOL	bShutdownFromDllMain;

	DWORD	dwMaxGameTime;
	BOOL	bBlockMinimize;
	BOOL	bQuitOnError;
	BOOL	bQuitOnHostile;
	BOOL	bStartAtMenu;
	BOOL	bActive;
	BOOL	bBlockKeys;
	BOOL	bBlockMouse;
	BOOL	bDisableCache;
	INT		dwMemUsage;

	Module* pModule;
	char	szPath[_MAX_PATH];
	char	szScriptPath[_MAX_PATH];

	WNDPROC oldWNDPROC;
	HHOOK hMouseHook;
	HHOOK hKeybHook;

	ImageHook *image;
	TextHook  *text;

	CCollisionMap cCollisionMap;

	CRITICAL_SECTION	cRoomSection;
	CRITICAL_SECTION	cMiscSection;
	CRITICAL_SECTION	cScreenhookSection;
	CRITICAL_SECTION	cPrintSection;
	CRITICAL_SECTION	cTextHookSection;
	CRITICAL_SECTION	cImageHookSection;
	CRITICAL_SECTION	cBoxHookSection;
	CRITICAL_SECTION	cFrameHookSection;
	CRITICAL_SECTION	cLineHookSection;
	CRITICAL_SECTION	cFlushCacheSection;
	CRITICAL_SECTION	cConsoleSection;

	DWORD dwSelectedUnitId;
	DWORD dwSelectedUnitType;
};

extern Variables Vars;
extern HANDLE hMemory;

BOOL Startup(void);
void Shutdown(void);

#define		INITIAL_HEAP_SIZE   4096    // initial heap size


inline void * __cdecl operator new(unsigned int nSize)
{
	if  (hMemory == NULL)   // if NULL, needs to be created
	{
		hMemory = HeapCreate(0,sizeof(char)*16384,sizeof(char)*51200);       // create heap

		if  (hMemory == NULL)
		{
			return NULL;    // HeapCreate failed
		}
	}

	// return pointer to allocated memory
	return	HeapAlloc(hMemory, 0, nSize);
};

inline void __cdecl operator delete(void *pObj)
{
	HeapFree(hMemory, 0, pObj);
};


#endif
