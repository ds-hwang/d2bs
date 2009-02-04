#ifndef PARTY_H
#define PARTY_H

#include "js32.h"

JSBool party_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

#define DEFFNC(f) JSBool f (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

DEFFNC(party_getNext)

#undef DEFFNC

/**********************************************************

					Classes/Objects/Methods

 **********************************************************/

static JSClass party_class = {
    "Party",	JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, party_getProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

enum party_tinyid {
	PARTY_AREA,
	PARTY_X,
	PARTY_Y,
	PARTY_GID,
	PARTY_LIFE,
	PARTY_NAME,
	PARTY_FLAG,
	PARTY_ID,
	PARTY_CLASSID,
	PARTY_LEVEL
};


static JSPropertySpec party_props[] = {
	{"x",			PARTY_X,			JSPROP_PERMANENT_VAR},
	{"y",			PARTY_Y,			JSPROP_PERMANENT_VAR},
	{"area",		PARTY_AREA,			JSPROP_PERMANENT_VAR},
	{"gid",			PARTY_GID,			JSPROP_PERMANENT_VAR},
	{"life",		PARTY_LIFE,			JSPROP_PERMANENT_VAR},
	{"partyflag",	PARTY_FLAG,			JSPROP_PERMANENT_VAR},
	{"partyid",		PARTY_ID,			JSPROP_PERMANENT_VAR},
	{"name",		PARTY_NAME,			JSPROP_PERMANENT_VAR},
	{"classid",		PARTY_CLASSID,		JSPROP_PERMANENT_VAR},
	{"level",		PARTY_LEVEL,		JSPROP_PERMANENT_VAR},
	{0},
};

static JSFunctionSpec party_methods[] = {
	{"getNext",			party_getNext,		0},
	{0},
};

#endif