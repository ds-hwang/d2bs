#ifndef __D2BS_H__
#define __D2BS_H__

#pragma once

#define XP_WIN
#define JS_THREADSAFE

#ifdef _MSVC_DEBUG

#undef _DEBUG_NEW_REDEFINE_NEW
#define _DEBUG_NEW_REDEFINE_NEW 1
#undef _DEBUG_NEW_TAILCHECK
#define _DEBUG_NEW_TAILCHECK 4
#undef _DEBUG_NEW_PROGNAME
#define _DEBUG_NEW_PROGNAME "D2BS"

#else

#undef _DEBUG_NEW_REDEFINE_NEW
#define _DEBUG_NEW_REDEFINE_NEW 0
#undef _DEBUG_NEW_TAILCHECK
#define _DEBUG_NEW_TAILCHECK 0

//#define new new (std::nothrow)

#endif

#ifdef DEBUG
#define D2BS_VERSION "$Rev$"
#else
#define D2BS_VERSION "1.1.1"
#endif

#include <windows.h>

#include "CollisionMap.h"
#include "ScreenHook.h"

struct Variables;

#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))

#define PRIVATE_UNIT		1
#define PRIVATE_CONTROL		3

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

	DWORD	dwMaxGameTime;
	BOOL	bDebug;
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

	// Critical Sections
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

BOOL Startup(void);
void Shutdown(void);

#endif
