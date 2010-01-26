#ifndef __JSDIRECTORY_H__
#define __JSDIRECTORY_H__

// TODO: Rewrite this mess of crap too

#include "js32.h"
#include <cstdlib>
#include <cstring>

#define DEFFNC(x) JSBool x (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

DEFFNC(dir_getFiles)
DEFFNC(dir_getDirectories)
DEFFNC(dir_create)
DEFFNC(dir_delete)
DEFFNC(my_openDir)

JSBool dir_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);;
void dir_finalize(JSContext *cx, JSObject *obj);

#undef DEFFNC

//////////////////////////////////////////////////////////////////
// directory stuff
//////////////////////////////////////////////////////////////////

static JSClass directory_class = {
	"Directory", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, dir_getProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, dir_finalize,
	JSCLASS_NO_OPTIONAL_MEMBERS
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
