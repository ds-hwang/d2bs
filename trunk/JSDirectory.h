#ifndef __JSDIRECTORY_H__
#define __JSDIRECTORY_H__

// TODO: Rewrite this mess of crap too

#include "js32.h"
#include <cstdlib>
#include <cstring>

CLASS_CTOR(dir);

JSAPI_FUNC(dir_getFiles);
JSAPI_FUNC(dir_getDirectories);
JSAPI_FUNC(dir_create);
JSAPI_FUNC(dir_delete);
JSAPI_FUNC(my_openDir);

JSAPI_PROP(dir_getProperty);
void dir_finalize(JSContext *cx, JSObject *obj);


//////////////////////////////////////////////////////////////////
// directory stuff
//////////////////////////////////////////////////////////////////

static JSClass directory_class = {
	"Directory", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, dir_getProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, dir_finalize,
    NULL, NULL, NULL, dir_ctor
};

enum {DIR_NAME};

static JSPropertySpec dir_props[] = {
	{"name",	DIR_NAME,	JSPROP_PERMANENT_VAR},
	{0}
};

static JSFunctionSpec dir_methods[] = {
	{"create",			dir_create,			1},
	{"remove",			dir_delete,			1},
	{"getFiles",		dir_getFiles,		1},
	{"getDirectories",	dir_getDirectories,	1},
	{0}
};

class DirData {
public:
	char name[_MAX_FNAME];
	DirData(char* newname)
	{
		strcpy_s(name, _MAX_FNAME, newname);
	}
};

#endif
