#ifndef __D2BS_H__
#define __D2BS_H__

#pragma once
#define XP_WIN
#define JS_THREADSAFE

#define D2BS_VERSION "1.1"


#pragma comment(lib, "shlwapi.lib")

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <ctime>
#include <math.h>
#include <direct.h>
#include <stdlib.h>
#include <errno.h>
#include <io.h>
#include <string.h>
#include <vector>

struct Variables;

#include "Offset.h"
#include "D2Structs.h"
#include "D2Ptrs.h"
#include "D2Skills.h"
#include "D2Helpers.h"
#include "D2Handlers.h"
#include "D2Intercepts.h"
#include "Constants.h"
#include "Common.h"
#include "Matrix.h"
#include "CollisionMap.h"
#include "TeleportPath.h"
#include "WalkPath.h"
#include "Helpers.h"
#include "dde.h"

#include "Core.h"
#include "Unit.h"
#include "ScreenHook.h"
#include "Events.h"
#include "Control.h"
#include "MPQStats.h"

extern Variables Vars;

#define ArraySize(x) (sizeof(x) / sizeof(x[0]))

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

	Module* pModule;
	char	szPath[_MAX_PATH];
	char	szScriptPath[_MAX_PATH];

	WNDPROC oldWNDPROC;
	HHOOK hMouseHook;
	HHOOK hKeybHook;
	CCollisionMap* cMap;

	// Critical Sections
	CRITICAL_SECTION	cRoomSection;
	CRITICAL_SECTION	cMiscSection;
	CRITICAL_SECTION	cScreenhookSection;
	CRITICAL_SECTION	cPrintSection;

	DWORD dwSelectedUnitId;
	DWORD dwSelectedUnitType;
};

#include "CriticalSections.h"
#include "cDebug.h"

#endif
