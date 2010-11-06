#pragma once

#include "js32.h"
#include "JSFile.h"
#include "JSSQLite.h"
#include "JSSandbox.h"
#include "JSUnit.h"
#include "JSScreenHook.h"
#include "JSPresetUnit.h"
#include "JSDirectory.h"
#include "JSFileTools.h"
#include "JSArea.h"
#include "JSControl.h"
#include "JSParty.h"
#include "JSExits.h"
#include "JSRoom.h"

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
	{&unit_class_ex.base,	unit_props,			unit_methods,			NULL,			NULL},
	{&presetunit_class,		presetunit_props,	NULL,					NULL,			NULL},
	{&area_class,			area_props,			NULL,					NULL,			NULL},
	{&control_class,		control_props,		control_funcs,			NULL,			NULL},
	{&folder_class,			dir_props,			dir_methods,			NULL,			NULL},
	{&exit_class,			exit_props,			NULL,					NULL,			NULL},
	{&party_class,			party_props,		party_methods,			NULL,			NULL},
	{&room_class,			room_props,			room_methods,			NULL,			NULL},

	// utility objects
	{&file_class_ex.base,	file_props,			file_methods,			NULL,			file_s_methods},
	{&sqlite_db_ex.base,	sqlite_props,		sqlite_methods,			NULL,			NULL},
	{&sqlite_stmt,			sqlite_stmt_props,	sqlite_stmt_methods,	NULL,			NULL},
	{&filetools_class,		NULL,				NULL,					NULL,			filetools_s_methods},
	{&sandbox_class,		NULL,				sandbox_methods,		NULL,			NULL},

	// screenhook objects
	{&frame_class,			frame_props,		frame_methods,			NULL,			NULL},
	{&box_class,			box_props,			box_methods,			NULL,			NULL},
	{&line_class,			line_props,			line_methods,			NULL,			NULL},
	{&text_class,			text_props,			text_methods,			NULL,			NULL},
	{&image_class,			image_props,		image_methods,			NULL,			NULL},
	{0}
};
