#include <Windows.h>
#include <io.h>

#include "Modules.hpp"
#include "ScriptEngine.hpp"
#include "Script.hpp"
#include "JSClasses.hpp"

JSFunctionSpec module_methods[] = {
	JS_FS("load", mod_load, 1, 0),
	JS_FS("include", mod_include, 1, 0),
	{0},
};

JSPropertySpec module_props[] = {
	{"searchPath", 0, JSPROP_STATIC, get_mod_sp, set_mod_sp},
	{0},
};

JSAPI_FUNC(mod_load)
{
	JSString* farg = NULL;
	if(!JS_ConvertArguments(cx, argc, vp, "S", &farg))
		return JS_FALSE;
	const jschar* file = JS_GetStringCharsZ(cx, farg);

	jsval sp = JSVAL_VOID;
	JSObject* obj = JS_THIS_OBJECT(cx, vp);
	if(!JS_GetProperty(cx, obj, "searchPath", &sp))
		THROW_ERROR(cx, obj, "Search path not found");

	JSString* str = JSVAL_TO_STRING(sp);
	JS::Anchor<JSString*> anchor(str);

	jschar* search = _wcsdup(JS_GetStringCharsZ(cx, str));
	wchar_t* contents = NULL;
	wchar_t* token = wcstok_s(search, L";", &contents);
	bool found = false;
	Script* script = (Script*)JS_GetContextPrivate(cx);

	while(token != NULL && !found)
	{
		size_t len = wcslen(token) + wcslen(file) + 1;
		wchar_t* path = new wchar_t[len];
		swprintf_s(path, len, L"%s\\%s", token, file);

		if(script->GetEngine()->ExistsInPath(path)) found = true;

		if(found)
		{
			Script* scr = script->GetEngine()->Compile(path);
			scr->Start();
		}
		else
		{
			token = wcstok_s(NULL, L";", &contents);
		}

		delete[] path;
	}

	free(search);
	return JS_TRUE;
}

JSAPI_FUNC(mod_include)
{
	JSString* farg = NULL;
	if(!JS_ConvertArguments(cx, argc, vp, "S", &farg))
		return JS_FALSE;
	const jschar* file = JS_GetStringCharsZ(cx, farg);

	jsval sp = JSVAL_VOID;
	JSObject* obj = JS_THIS_OBJECT(cx, vp);
	if(!JS_GetProperty(cx, obj, "searchPath", &sp))
		THROW_ERROR(cx, obj, "Search path not found");

	JSString* str = JSVAL_TO_STRING(sp);
	JS::Anchor<JSString*> anchor(str);

	jschar* search = _wcsdup(JS_GetStringCharsZ(cx, str));
	wchar_t* contents = NULL;
	wchar_t* token = wcstok_s(search, L";", &contents);
	bool found = false;
	Script* script = (Script*)JS_GetContextPrivate(cx);

	while(token != NULL && !found)
	{
		size_t len = wcslen(token) + wcslen(file) + 1;
		wchar_t* path = new wchar_t[len];
		swprintf_s(path, len, L"%s\\%s", token, file);

		if(script->GetEngine()->ExistsInPath(path)) found = true;

		if(found)
		{
			std::string cpath;
			std::wstring wpath(path);
			cpath.assign(wpath.begin(), wpath.end());

			JSContext* ctxt = script->GetEngine()->GetContext();
			JSObject* gobj = JS_NewCompartmentAndGlobalObject(ctxt, &global, NULL);
			JSObject* scr = JS_CompileFile(ctxt, gobj, cpath.c_str());
			jsval result;
			JS_ExecuteScript(ctxt, gobj, scr, &result);
			script->GetEngine()->ReleaseContext(ctxt);

			auto compartment = JS_EnterCrossCompartmentCall(cx, JSVAL_TO_OBJECT(result));
			JS_SET_RVAL(cx, vp, result);
			JS_LeaveCrossCompartmentCall(compartment);
		}
		else
		{
			token = wcstok_s(NULL, L";", &contents);
		}


		delete[] path;
	}

	free(search);

	return JS_TRUE;
}

JSAPI_PROP(get_mod_sp)
{
	jsval path = JSVAL_VOID;
	JS_GetProperty(cx, obj, "searchPath", &path);
	JS_SET_RVAL(cx, vp, path);
	return JS_TRUE;
}

JSAPI_STRICT_PROP(set_mod_sp)
{
	jsval path = vp[0];
	JS_SetProperty(cx, obj, "searchPath", &path);
	return JS_TRUE;
}
