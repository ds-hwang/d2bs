#include <Windows.h>
#include <io.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "Modules.hpp"
#include "Engine.hpp"
#include "Script.hpp"

#include "JSClasses.hpp"

JSFunctionSpec module_methods[] = {
	JS_FS("load",		mod_load,		1, JSPROP_STATIC),
	JS_FS("require",	mod_require,	1, JSPROP_STATIC),
	{0}
};

JSAPI_FUNC(mod_load)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	JSString* farg = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &farg))
		return JS_FALSE;
	const jschar* file = JS_GetStringCharsZ(cx, farg);

	std::vector<jsval> paths;
	if(!JS_ArrayToVector(cx, script->GetSearchPath(), paths))
		return JS_FALSE;

	// check the current directory last
	paths.push_back(JS_GetEmptyStringValue(cx));

	auto end = paths.end();
	for(auto it = paths.begin(); it != end; it++)
	{
		std::wstring path;
		std::wstring rel = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(*it));
		if(rel != L"") path = rel + L"\\" + file;
		else path = file;

		if(script->GetEngine()->Exists(path.c_str()))
		{
			// TODO: wrap the resultant Script in an object and return it to the caller
			script->GetEngine()->CompileScript(path.c_str())->Start();
			break;
		}
	}

	return JS_TRUE;
}

JSAPI_FUNC(mod_require)
{
	Script* script = (Script*)JS_GetContextPrivate(cx);
	JS_SET_RVAL(cx, vp, JSVAL_VOID);

	JSString* farg = nullptr;
	if(!JS_ConvertArguments(cx, argc, JS_ARGV(cx, vp), "S", &farg))
		return JS_FALSE;
	const jschar* file = JS_GetStringCharsZ(cx, farg);

	std::vector<jsval> paths;
	if(!JS_ArrayToVector(cx, script->GetSearchPath(), paths))
		return JS_FALSE;

	// check the current directory last
	paths.push_back(JS_GetEmptyStringValue(cx));

	auto end = paths.end();
	for(auto it = paths.begin(); it != end; it++)
	{
		std::wstring path;
		std::wstring rel = JS_GetStringCharsZ(cx, JSVAL_TO_STRING(*it));
		if(rel != L"") path = rel + L"\\" + file;
		else path = file;

		if(script->GetEngine()->Exists(path.c_str(), true) || script->GetEngine()->Exists(path.c_str()))
		{
			Module* module = script->GetEngine()->CompileModule(cx, path.c_str());
			if(module->IsReady()) JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(module->GetExports()));
			else return JS_FALSE;
		}
	}

	return JS_TRUE;
}
