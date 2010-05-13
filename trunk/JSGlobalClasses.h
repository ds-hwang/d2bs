#pragma once

#include "js32.h"

#include "JSFile.h"
#include "JSSQLite.h"
#include "JSSandbox.h"

struct JSClassSpec {
	JSClass* js_class;
	JSPropertySpec* props;
	JSFunctionSpec* funcs;
	JSPropertySpec* static_props;
	JSFunctionSpec* static_funcs;
};

static JSClassSpec global_classes[] = {
	/*JSClass*				properties			functions				static props	static funcs */
	// game objects

	// utility objects
	{&file_class_ex.base,	file_props,			file_methods,			NULL,			file_s_methods},
	{&sqlite_db_ex.base,	sqlite_props,		sqlite_methods,			NULL,			NULL},
	{&sqlite_stmt,			sqlite_stmt_props,	sqlite_stmt_methods,	NULL,			NULL},
	{&filetools_class,		NULL,				NULL,					NULL,			filetools_s_methods},
	{&sandbox_class,		NULL,				sandbox_methods,		NULL,			NULL},

	// screenhook objects
	{0}
};
