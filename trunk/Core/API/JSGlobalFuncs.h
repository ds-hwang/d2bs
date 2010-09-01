#pragma once

#include "js32.h"
#include "JSCore.h"
#include "JSHash.h"

#include "JSFile.h"
#include "JSFileTools.h"
#include "JSDirectory.h"
#include "JSSQLite.h"
#include "JSSandbox.h"
#include "JSScript.h"

static JSFunctionSpec global_funcs[] = {
	// "get" functions

	// utility functions that don't have anything to do with the game
	{"print",				my_print,				1},
	{"sleep",				my_sleep,				1},
	{"load",				my_load,				1},
	{"isIncluded",			my_isIncluded,			1},
	{"include",				my_include,				1},
	{"stop",				my_stop,				0},
	{"rand",				my_rand,				0},
	{"sendCopyData",		my_sendCopyData,		4},
	{"sendDDE",				my_sendDDE,				0},
	{"addEventListener",	my_addEventListener,	2},
	{"removeEventListener",	my_removeEventListener,	2},
	{"clearEvent",			my_clearEvent,			1},
	{"clearAllEvents",		my_clearAllEvents,		0},
	{"scriptBroadcast",		my_scriptBroadcast,		1},
	{"dopen",				my_openDir,				1},
	{"debugLog",			my_debugLog,			1},
	{"showConsole",			my_showConsole,			0},
	{"hideConsole",			my_hideConsole,			0},
	{"clearConsole",		my_clearConsole,		0},

	// out of game functions

	// game functions that don't have anything to do with gathering data

	// drawing functions
	
	// hash functions
	{"md5",					my_md5,					1},
	{"sha1",				my_sha1,				1},
	{"sha256",				my_sha256,				1},
	{"sha384",				my_sha384,				1},
	{"sha512",				my_sha512,				1},
	{"md5_file",			my_md5_file,			1},
	{"sha1_file",			my_sha1_file,			1},
	{"sha256_file",			my_sha256_file,			1},
	{"sha384_file",			my_sha384_file,			1},
	{"sha512_file",			my_sha512_file,			1},
	{0}
};
