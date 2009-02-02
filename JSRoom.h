#ifndef ROOM_H
#define ROOM_H

INT room_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

#define DEFFNC(f) INT f (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

DEFFNC(room_getNext)
DEFFNC(room_getPresetUnits)
DEFFNC(room_getCollision)
DEFFNC(room_getNearby)
DEFFNC(room_getStat)
DEFFNC(room_getFirst)
DEFFNC(room_unitInRoom)
DEFFNC(room_reveal)

#undef DEFFNC

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

enum room_tinyid {
	ROOM_NUM, 
	ROOM_XPOS, 
	ROOM_YPOS, 
	ROOM_XSIZE, 
	ROOM_YSIZE, 
	ROOM_SUBNUMBER, 
	ROOM_AREA, 
	ROOM_LEVEL, 
	ROOM_CORRECTTOMB,
};

static JSPropertySpec room_props[] = { 
	{"number",		ROOM_NUM,			JSPROP_PERMANENT_VAR},
	{"x",			ROOM_XPOS,			JSPROP_PERMANENT_VAR},
	{"y",			ROOM_YPOS,			JSPROP_PERMANENT_VAR},
	{"xsize",		ROOM_XSIZE,			JSPROP_PERMANENT_VAR},
	{"ysize",		ROOM_YSIZE,			JSPROP_PERMANENT_VAR},
	{"subnumber",	ROOM_SUBNUMBER,		JSPROP_PERMANENT_VAR},
	{"area",		ROOM_AREA,			JSPROP_PERMANENT_VAR},
	{"level",		ROOM_LEVEL,			JSPROP_PERMANENT_VAR},
	{"correcttomb",	ROOM_CORRECTTOMB,	JSPROP_PERMANENT_VAR},
	{0}
};

static JSFunctionSpec room_methods[] = {
	{"getNext",			room_getNext,			0},
	{"getPresetUnits",	room_getPresetUnits,	0},
	{"getCollision",	room_getCollision,		0},
	{"getNearby",		room_getNearby,			0},
	{"getStat",			room_getStat,			0},
	{"getFirst",		room_getFirst,			0},
	{"unitInRoom",		room_unitInRoom,		1},
	{"reveal",			room_reveal,			0},
	{0}
};

static JSClass room_class = {
    "Room", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, room_getProperty, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

#endif