#pragma once

#include "js32.h"
#include "JSCore.h"
#include "JSFile.h"
#include "JSFileTools.h"
#include "JSDirectory.h"
#include "JSSQLite.h"
#include "JSSandbox.h"
#include "JSScreenHook.h"
#include "JSUnit.h"

static JSFunctionSpec global_funcs[] = {
	{"getUnit",				unit_getUnit,			1},
	{"getPath",				my_getPath,				0},
	{"getCollision",		my_getCollision,		0},
	{"getMercHP",			my_getMercHP,			0},
	{"getCursorType",		my_getCursorType,		0},
	{"getSkillByName",		my_getSkillByName,		1},
	{"getSkillById",		my_getSkillById,		1},
	{"getLocaleString",		my_getLocaleString,		1},
	{"getTextWidthHeight",	my_getTextWidthHeight,	2},
	{"getThreadPriority",	my_getThreadPriority,	0},
	{"getUIFlag",			my_getUIFlag,			1},
	{"getTradeInfo",		my_getTradeInfo,		0},
	{"getWaypoint",			my_getWaypoint,			1},
	{"getScript",			my_getScript,			0},
	{"getRoom",				my_getRoom,				0},
	{"getParty",			my_getParty,			0},
	{"getPresetUnit",		my_getPresetUnit,		0},
	{"getPresetUnits",		my_getPresetUnits,		0},
	{"getArea",				my_getArea,				0},
	{"getExits",			my_getExits,			0},
	{"getBaseStat",			my_getBaseStat,			0},
	{"getControl",			my_getControl,			0},
	{"getPlayerFlag",		my_getPlayerFlag,		2},
	{"getTickCount",		my_getTickCount,		0},
	{"debugLog",			my_debugLog,			1},
	{"getInteractedNPC",	my_getInteractedNPC,	0},

	{"submitItem",			my_submitItem,			0},
	{"print",				my_print,				1},
	{"delay",				my_delay,				1},
	{"load",				my_load,				1},
	{"include",				my_include,				1},
	{"login",				my_login,				1},
	{"getMouseCoords",		my_getMouseCoords,		1},
	{"copyUnit",			my_copyUnit,			1},
	{"stop",				my_stop,				0},
	{"clickMap",			my_clickMap,			3},
	{"acceptTrade",			my_acceptTrade,			0},
	{"beep",				my_beep,				0},
	{"clickItem",			my_clickItem,			0},
	{"rnd",					my_rnd,					0},
	{"getDistance",			my_getDistance,			2},
	{"gold",				my_gold,				1},
	{"checkCollision",		my_checkCollision,		2},
	{"isIncluded",			my_isIncluded,			1},
	{"playSound",			my_playSound,			1},
	{"quit",				my_quit,				0},
	{"quitGame",			my_quitGame,			0},
	{"runGC",				my_GC,					0},
	{"say",					my_say,					1},
	{"sendCopyData",		my_sendCopyData,		4},
	{"sendDDE",				my_sendDDE,				0},
	{"keystate",			my_keystate,			0},
	{"addEventListener",	my_addEventListener,	2},
	{"removeEventListener",	my_removeEventListener,	2},
	{"clearEvent",			my_clearEvent,			1},
	{"clearAllEvents",		my_clearAllEvents,		0},
	{"clickParty",			my_clickParty,			1},
	{"blockMinimize",		my_blockMinimize,		0},
	{"js_strict",			my_js_strict,			0},
	{"weaponSwitch",		my_weaponSwitch,		0},
	{"version",				my_version,				0},
	{"transmute",			my_transmute,			0},
	{"scriptBroadcast",		my_scriptBroadcast,		1},
	{"iniread",				my_iniread,				4},
	{"iniwrite",			my_iniwrite,			4},
	{"sqlite_version",		my_sqlite_version,		0},
	{"dopen",				openDir,				1},
	{0}
};
