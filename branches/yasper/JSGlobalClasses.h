#pragma once

#include "js32.h"
#include "JSFile.h"
#include "JSSQLite.h"
#include "JSSandbox.h"
#include "JSUnit.h"
#include "JSScreenHook.h"

struct JSClassSpec {
	JSClass* js_class;
	JSNative class_ctor;
	JSPropertySpec* props;
	JSFunctionSpec* funcs;
	JSPropertySpec* static_props;
	JSFunctionSpec* static_funcs;
};

static JSClassSpec global_classes[] = {
	/*JSClass*				ctor			properties			functions				static props	static funcs */
	{&file_class_ex.base,	file_ctor,		file_props,			file_methods,			NULL,			file_s_methods},
	{&sqlite_db_ex.base,	sqlite_ctor,	sqlite_props,		sqlite_methods,			NULL,			NULL},
	{&sqlite_stmt,			NULL,			sqlite_stmt_props,	sqlite_stmt_methods,	NULL,			NULL},
	{&filetools_class,		NULL,			NULL,				NULL,					NULL,			filetools_s_methods},
	{&sandbox_class,		sandbox_ctor,	NULL,				sandbox_methods,		NULL,			NULL},
	{&frame_class,			frame_ctor,		frame_props,		frame_methods,			NULL,			NULL},
	{&box_class,			box_ctor,		box_props,			box_methods,			NULL,			NULL},
	{&line_class,			line_ctor,		line_props,			line_methods,			NULL,			NULL},
	{&text_class,			text_ctor,		text_props,			text_methods,			NULL,			NULL},
	{&image_class,			image_ctor,		image_props,		image_methods,			NULL,			NULL},
	{&unit_class,			NULL,			unit_props,			unit_methods,			NULL,			NULL},
	{0}
};
