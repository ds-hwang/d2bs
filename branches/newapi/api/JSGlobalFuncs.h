#pragma once

#ifndef __JSGLOBALFUNCS_H__
#define __JSGLOBALFUNCS_H__

#include "js32.h"

// TODO: Anywhere we can put these that makes sense?
JSAPI_FUNC(global_log);
JSAPI_FUNC(global_sleep);

// TODO: Move these to the Core or Script object
JSAPI_FUNC(core_abort);
JSAPI_FUNC(core_abortAll);
JSAPI_FUNC(core_addEvent);
JSAPI_FUNC(core_removeEvent);
JSAPI_FUNC(core_clearEvent);
JSAPI_FUNC(core_clearAllEvents);

// TODO: Figure out where to put these, but they shouldn't be global
JSAPI_FUNC(script_include);
JSAPI_FUNC(script_isIncluded);
JSAPI_FUNC(script_addIncludePath);

static JSFunctionSpec global_funcs[] = {
	{"log",				global_log,				1},
	{"sleep",			global_sleep,			1},

	{"abort",			core_abort,				0},
	{"abortAll",		core_abortAll,			0},
	{"addEvent",		core_addEvent,			2},
	{"removeEvent",		core_removeEvent,		2},
	{"clearEvent",		core_clearEvent,		1},
	{"clearAllEvents",	core_clearAllEvents,	0},

	{"include",			script_include,			1},
	{"isIncluded",		script_isIncluded,		1},
	{"addIncludePath",	script_addIncludePath,	1},
	{0}
};

#endif
