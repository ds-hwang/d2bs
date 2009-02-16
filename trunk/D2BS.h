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

#define D2BS_VERSION "1.1"

#include <windows.h>
#include "CollisionMap.h"
#include "ScreenHook.h"

struct Variables;

#define ArraySize(x) (sizeof((x)) / sizeof((x)[0]))

#define PRIVATE_UNIT		1
#define PRIVATE_CONTROL		3

enum {
	EVENT_AREACHANGE = 1,
	EVENT_CHATMSG,
	EVENT_COPYDATA,
	EVENT_GAMEMSG,
	EVENT_HOSTILEMSG,
	EVENT_INPUTLINE,
	EVENT_ITEMSTAT,
	EVENT_KEYDOWN,
	EVENT_KEYUP,
	EVENT_MELIFE,
	EVENT_MISSILEMOVE,
	EVENT_MISSILESTATE,
	EVENT_MOUSEDOWN,
	EVENT_MOUSEUP,
	EVENT_NEWITEM,
	EVENT_NEWNPC,
	EVENT_NPCLIFE,
	EVENT_NPCMOVE,
	EVENT_NPCSTAT,
	EVENT_NPCSTATE,
	EVENT_PARTYMSG,
	EVENT_PLAYERMOVE,
	EVENT_PLAYERSTAT,
	EVENT_PLAYERSTATE,
	EVENT_QUEST,
	EVENT_SCRIPTMSG,
	EVENT_UNITMOVE,
	EVENT_WINDOWFOCUS,
	EVENT_CHATCMD,
	EVENT_PLAYERASSIGN
};

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

	DWORD	dwMaxGameTime;
	BOOL	bDebug;
	BOOL	bBlockMinimize;
	BOOL	bQuitOnError;
	BOOL	bQuitOnHostile;
	BOOL	bStartAtMenu;
	BOOL	bActive;

	Module* pModule;
	char	szPath[_MAX_PATH];
	char	szScriptPath[_MAX_PATH];

	WNDPROC oldWNDPROC;
	HHOOK hMouseHook;
	HHOOK hKeybHook;
	CCollisionMap* cMap;

	ImageHook* image;
	TextHook* text;

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

	DWORD dwSelectedUnitId;
	DWORD dwSelectedUnitType;
};

extern Variables Vars;

#endif
