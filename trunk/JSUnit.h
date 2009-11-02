#pragma once

#include <windows.h>
#include "js32.h"

JSAPI_FUNC(unit_getUnit);
JSAPI_FUNC(unit_getNext);
JSAPI_FUNC(unit_cancel);
JSAPI_FUNC(unit_repair);
JSAPI_FUNC(unit_useMenu);
JSAPI_FUNC(unit_interact);
JSAPI_FUNC(unit_getStat);
JSAPI_FUNC(unit_getState);
JSAPI_FUNC(unit_getItems);
JSAPI_FUNC(unit_getSkill);
JSAPI_FUNC(unit_getParent);
JSAPI_FUNC(unit_setskill);
JSAPI_FUNC(unit_getMerc);
JSAPI_FUNC(unit_getItem);
JSAPI_FUNC(unit_move);
JSAPI_FUNC(item_getFlag);
JSAPI_FUNC(item_getFlags);
JSAPI_FUNC(item_getPrice);
JSAPI_FUNC(item_shop);
JSAPI_FUNC(my_overhead);
JSAPI_FUNC(unit_getEnchant);
JSAPI_FUNC(unit_getQuest);
JSAPI_FUNC(unit_getMinionCount);

void unit_finalize(JSContext *cx, JSObject *obj);
JSBool unit_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
JSBool unit_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

struct myUnit
{
	DWORD _dwPrivateType;
	DWORD dwUnitId;
	DWORD dwClassId;
	DWORD dwType;
	DWORD dwMode;
	CHAR szName[128];
};

enum unit_tinyid {
	UNIT_TYPE, UNIT_CLASSID, UNIT_ID, UNIT_MODE, UNIT_NAME, UNIT_BUSY,
	UNIT_ACT, UNIT_XPOS, UNIT_YPOS, ME_WSWITCH, UNIT_AREA,
	UNIT_HP, UNIT_HPMAX, UNIT_MP, UNIT_MPMAX, UNIT_STAMINA, UNIT_STAMINAMAX,
	UNIT_CHARLVL, ME_RUNWALK, UNIT_ADDRESS, ITEM_CODE, ITEM_PREFIX,
	ITEM_SUFFIX, ITEM_FNAME, ITEM_QUALITY, ITEM_NODE, UNIT_SELECTABLE, ITEM_LOC,
	ITEM_SIZEX, ITEM_SIZEY, ITEM_TYPE, MISSILE_DIR, MISSILE_VEL, ITEM_CLASS,
	UNIT_SPECTYPE, ITEM_DESC, ITEM_BODYLOCATION, UNIT_ITEMCOUNT, ITEM_LEVELREQ,
	UNIT_OWNER, UNIT_OWNERTYPE, UNIT_UNIQUEID, ITEM_LEVEL, UNIT_DIRECTION, ITEM_SUFFIXNUM, ITEM_PREFIXNUM,
	OBJECT_TYPE, OBJECT_LOCKED
};

static JSClass unit_class = {
    "Unit",	JSCLASS_CONSTRUCT_PROTOTYPE | JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, unit_getProperty, unit_setProperty,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, unit_finalize,
    NULL, NULL, 
};
enum me_tinyid {
	ME_ACCOUNT = 100,
	ME_CHARNAME,
	ME_CHICKENHP,
	ME_CHICKENMP,
	ME_DIFF,
	ME_GAMENAME,
	ME_GAMEPASSWORD,
	ME_GAMESERVERIP,
	ME_GAMESTARTTIME,
	ME_GAMETYPE,
	ME_ITEMONCURSOR,
	ME_LADDER,
	ME_PING,
	ME_PLAYERTYPE,
	ME_QUITONHOSTILE,
	ME_REALM,
	ME_REALMSHORT,
	ME_MERCREVIVECOST,
	OOG_WINDOWTITLE,
	OOG_DEBUG,
	OOG_SCREENSIZE,
	OOG_INGAME,
	OOG_QUITONERROR,
	OOG_MAXGAMETIME,
	ME_BLOCKKEYS,
	ME_BLOCKMOUSE
};

static JSPropertySpec me_props[] = {
	{"account",			ME_ACCOUNT,			JSPROP_PERMANENT_VAR},
	{"charname",		ME_CHARNAME,		JSPROP_PERMANENT_VAR},
	{"diff",			ME_DIFF,			JSPROP_PERMANENT_VAR},
	{"gamename",		ME_GAMENAME,		JSPROP_PERMANENT_VAR},
	{"gamepassword",	ME_GAMEPASSWORD,	JSPROP_PERMANENT_VAR},
	{"gameserverip",	ME_GAMESERVERIP,	JSPROP_PERMANENT_VAR},
	{"gamestarttime",	ME_GAMESTARTTIME,	JSPROP_PERMANENT_VAR},
	{"gametype",		ME_GAMETYPE,		JSPROP_PERMANENT_VAR},
	{"itemoncursor",	ME_ITEMONCURSOR,	JSPROP_PERMANENT_VAR},
	{"ladder",			ME_LADDER,			JSPROP_PERMANENT_VAR},
	{"ping",			ME_PING,			JSPROP_PERMANENT_VAR},
	{"playertype",		ME_PLAYERTYPE,		JSPROP_PERMANENT_VAR},
	{"realm",			ME_REALM,			JSPROP_PERMANENT_VAR},
	{"realmshort",		ME_REALMSHORT,		JSPROP_PERMANENT_VAR},
	{"mercrevivecost",	ME_MERCREVIVECOST,	JSPROP_PERMANENT_VAR},
	{"runwalk",			ME_RUNWALK,			JSPROP_PERMANENT_VAR},
	{"weaponswitch",	ME_WSWITCH,			JSPROP_PERMANENT_VAR},
	{"chickenhp",		ME_CHICKENHP,		JSPROP_STATIC_VAR},
	{"chickenmp",		ME_CHICKENMP,		JSPROP_STATIC_VAR},
	{"quitonhostile",	ME_QUITONHOSTILE,	JSPROP_STATIC_VAR}, // Goddamnit TPPK whores..
	{"blockKeys",		ME_BLOCKKEYS,		JSPROP_STATIC_VAR},
	{"blockMouse",		ME_BLOCKMOUSE,		JSPROP_STATIC_VAR},

	{"screensize",		OOG_SCREENSIZE,		JSPROP_PERMANENT_VAR},
	{"windowtitle",		OOG_WINDOWTITLE,	JSPROP_PERMANENT_VAR},
	{"ingame",			OOG_INGAME,			JSPROP_PERMANENT_VAR},
	{"quitonerror",		OOG_QUITONERROR,	JSPROP_STATIC_VAR},
	{"maxgametime",		OOG_MAXGAMETIME,	JSPROP_STATIC_VAR},
	{"debug",			OOG_DEBUG,			JSPROP_STATIC_VAR},

	{"type",			UNIT_TYPE,			JSPROP_PERMANENT_VAR},
	{"classid",			UNIT_CLASSID,		JSPROP_PERMANENT_VAR},
	{"mode",			UNIT_MODE,			JSPROP_PERMANENT_VAR},
	{"name",			UNIT_NAME,			JSPROP_PERMANENT_VAR},
	{"act",				UNIT_ACT,			JSPROP_PERMANENT_VAR},
	{"gid",				UNIT_ID,			JSPROP_PERMANENT_VAR},
	{"x",				UNIT_XPOS,			JSPROP_PERMANENT_VAR},
	{"y",				UNIT_YPOS,			JSPROP_PERMANENT_VAR},
	{"area",			UNIT_AREA,			JSPROP_PERMANENT_VAR},
	{"hp",				UNIT_HP,			JSPROP_PERMANENT_VAR},
	{"hpmax",			UNIT_HPMAX,			JSPROP_PERMANENT_VAR},
	{"mp",				UNIT_MP,			JSPROP_PERMANENT_VAR},
	{"mpmax",			UNIT_MPMAX,			JSPROP_PERMANENT_VAR},
	{"stamina",			UNIT_STAMINA,		JSPROP_PERMANENT_VAR},
	{"staminamax",		UNIT_STAMINAMAX,	JSPROP_PERMANENT_VAR},
	{"charlvl",			UNIT_CHARLVL,		JSPROP_PERMANENT_VAR},
	{"itemcount",		UNIT_ITEMCOUNT,		JSPROP_PERMANENT_VAR},
	{"owner",			UNIT_OWNER,			JSPROP_PERMANENT_VAR},
	{"ownertype",		UNIT_OWNERTYPE,		JSPROP_PERMANENT_VAR},
	{"spectype",		UNIT_SPECTYPE,		JSPROP_PERMANENT_VAR},
	{"direction",		UNIT_DIRECTION,		JSPROP_PERMANENT_VAR},

	{"code",			ITEM_CODE,			JSPROP_PERMANENT_VAR},
	{"prefix",			ITEM_PREFIX,		JSPROP_PERMANENT_VAR},
	{"suffix",			ITEM_SUFFIX,		JSPROP_PERMANENT_VAR},
	{"prefixnum",		ITEM_PREFIXNUM,		JSPROP_PERMANENT_VAR},
	{"suffixnum",		ITEM_SUFFIXNUM,		JSPROP_PERMANENT_VAR},
	{"fname",			ITEM_FNAME,			JSPROP_PERMANENT_VAR},
	{"quality",			ITEM_QUALITY,		JSPROP_PERMANENT_VAR},
	{"node",			ITEM_NODE,			JSPROP_PERMANENT_VAR},
	{"location",		ITEM_LOC,			JSPROP_PERMANENT_VAR},
	{"sizex",			ITEM_SIZEX,			JSPROP_PERMANENT_VAR},
	{"sizey",			ITEM_SIZEY,			JSPROP_PERMANENT_VAR},
	{"itemType",		ITEM_TYPE,			JSPROP_PERMANENT_VAR},
	{"description",		ITEM_DESC,			JSPROP_PERMANENT_VAR},
	{"bodylocation",	ITEM_BODYLOCATION,	JSPROP_PERMANENT_VAR},
	{"ilvl",			ITEM_LEVEL,			JSPROP_PERMANENT_VAR},
	{0},
};

static JSPropertySpec unit_props[] = {
	{"type",		UNIT_TYPE,			JSPROP_PERMANENT_VAR},
	{"classid",		UNIT_CLASSID,		JSPROP_PERMANENT_VAR},
	{"mode",		UNIT_MODE,			JSPROP_PERMANENT_VAR},
	{"name",		UNIT_NAME,			JSPROP_PERMANENT_VAR},
	{"act",			UNIT_ACT,			JSPROP_PERMANENT_VAR},
	{"gid",			UNIT_ID,			JSPROP_PERMANENT_VAR},
	{"x",			UNIT_XPOS,			JSPROP_PERMANENT_VAR},
	{"y",			UNIT_YPOS,			JSPROP_PERMANENT_VAR},
	{"area",		UNIT_AREA,			JSPROP_PERMANENT_VAR},
	{"hp",			UNIT_HP,			JSPROP_PERMANENT_VAR},
	{"hpmax",		UNIT_HPMAX,			JSPROP_PERMANENT_VAR},
	{"mp",			UNIT_MP,			JSPROP_PERMANENT_VAR},
	{"mpmax",		UNIT_MPMAX,			JSPROP_PERMANENT_VAR},
	{"stamina",		UNIT_STAMINA,		JSPROP_PERMANENT_VAR},
	{"staminamax",	UNIT_STAMINAMAX,	JSPROP_PERMANENT_VAR},
	{"charlvl",		UNIT_CHARLVL,		JSPROP_PERMANENT_VAR},
	{"itemcount",	UNIT_ITEMCOUNT,		JSPROP_PERMANENT_VAR},
	{"owner",		UNIT_OWNER,			JSPROP_PERMANENT_VAR},
	{"ownertype",	UNIT_OWNERTYPE,		JSPROP_PERMANENT_VAR},
	{"spectype",	UNIT_SPECTYPE,		JSPROP_PERMANENT_VAR},
	{"direction",	UNIT_DIRECTION,		JSPROP_PERMANENT_VAR},
	{"uniqueid",	UNIT_UNIQUEID,		JSPROP_PERMANENT_VAR},

	{"code",		ITEM_CODE,			JSPROP_PERMANENT_VAR},
	{"prefix",		ITEM_PREFIX,		JSPROP_PERMANENT_VAR},
	{"suffix",		ITEM_SUFFIX,		JSPROP_PERMANENT_VAR},
	{"prefixnum",	ITEM_PREFIXNUM,		JSPROP_PERMANENT_VAR},
	{"suffixnum",	ITEM_SUFFIXNUM,		JSPROP_PERMANENT_VAR},
	{"fname",		ITEM_FNAME,			JSPROP_PERMANENT_VAR},
	{"quality",		ITEM_QUALITY,		JSPROP_PERMANENT_VAR},
	{"node",		ITEM_NODE,			JSPROP_PERMANENT_VAR},
	{"location",	ITEM_LOC,			JSPROP_PERMANENT_VAR},
	{"sizex",		ITEM_SIZEX,			JSPROP_PERMANENT_VAR},
	{"sizey",		ITEM_SIZEY,			JSPROP_PERMANENT_VAR},
	{"itemType",	ITEM_TYPE,			JSPROP_PERMANENT_VAR},
	{"description", ITEM_DESC,			JSPROP_PERMANENT_VAR},
	{"bodylocation",ITEM_BODYLOCATION,	JSPROP_PERMANENT_VAR},
	{"ilvl",		ITEM_LEVEL,			JSPROP_PERMANENT_VAR},
	{"lvlreq",		ITEM_LEVELREQ,		JSPROP_PERMANENT_VAR},

	{"runwalk",		ME_RUNWALK,			JSPROP_PERMANENT_VAR},
	{"weaponswitch",ME_WSWITCH,			JSPROP_PERMANENT_VAR},

	{"objtype",		OBJECT_TYPE,		JSPROP_PERMANENT_VAR},
	{"islocked",	OBJECT_LOCKED,		JSPROP_PERMANENT_VAR},
	{0},
};

static JSFunctionSpec unit_methods[] = {
	{"getNext",			unit_getNext,		0},
	{"cancel",			unit_cancel,		0},
	{"repair",			unit_repair,		0},
	{"useMenu",			unit_useMenu,		0},
	{"interact",		unit_interact,		0},
	{"getItems",		unit_getItems,		0},
	{"getMerc",			unit_getMerc,		0},
	{"getSkill",		unit_getSkill,		0},
	{"getParent",		unit_getParent,		0},
	{"overhead",		my_overhead,		0},
	{"getFlags",		item_getFlags,		1},
	{"getFlag",			item_getFlag,		1},
	{"getStat",			unit_getStat,		1},
	{"getState",		unit_getState,		1},
	{"getPrice",		item_getPrice,		1},
	{"getItem",			unit_getItem,		1},
	{"getEnchant",		unit_getEnchant,	1},
	{"shop",			item_shop,			2},
	{"setSkill",		unit_setskill,		2},
	{"move",			unit_move,			2},
	{"getQuest",		unit_getQuest,		2},
	{"getMinionCount",	unit_getMinionCount, 1},
	{0},
};
