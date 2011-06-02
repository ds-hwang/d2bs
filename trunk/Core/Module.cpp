#include <algorithm>
#include <iterator>
#include <fstream>

#include "Engine.hpp"
#include "JSClasses.hpp"

Module::Module(JSContext* cx, const wchar_t* path, Engine* engine) :
	ready(false), cx(cx)
{
	std::string cpath;
	std::wstring wpath(path);
	cpath.assign(wpath.begin(), wpath.end());

	obj = JS_NewGlobalObject(cx, &global);
	JS_AddObjectRoot(cx, &obj);
	engine->InitClasses(cx, obj);

	exports = JS_NewObject(cx, nullptr, nullptr, obj);
	module = JS_NewObject(cx, nullptr, nullptr, obj);

	JS_DefineProperty(cx, module, "id", STRING_TO_JSVAL(JS_InternUCString(cx, path)), nullptr, nullptr, JSPROP_DEFAULT);

	JS_DefineProperty(cx, obj, "module", OBJECT_TO_JSVAL(module), nullptr, nullptr, JSPROP_DEFAULT);
	JS_DefineProperty(cx, obj, "exports", OBJECT_TO_JSVAL(exports), nullptr, nullptr, JSPROP_DEFAULT);

	std::string str(std::istreambuf_iterator<char>(std::ifstream(path).rdbuf()),std::istreambuf_iterator<char>());
	str = "(function(){ " + str + " })();";

	JSObject* scr = JS_CompileScript(cx, obj, str.c_str(), str.length(), cpath.c_str(), 1);
	if(scr == nullptr) ready = false;
	else {
		jsval result;
		ready = !!JS_ExecuteScript(cx, obj, scr, &result);
	}
}

Module::Module(JSContext* cx, JSObject* obj,JSModuleSpec* mod) :
	cx(cx), obj(obj), exports(nullptr), module(nullptr)
{
	exports = JS_NewObject(cx, nullptr, nullptr, obj);
	JS_AddObjectRoot(cx, &(this->obj));

	JS_DefineProperty(cx, obj, "exports", OBJECT_TO_JSVAL(exports), nullptr, nullptr, JSPROP_DEFAULT);

	if(mod->classes != nullptr) JS_DefineClasses(cx, exports, mod->classes);
	if(mod->static_methods != nullptr) JS_DefineFunctions(cx, exports, mod->static_methods);
	if(mod->static_properties != nullptr) JS_DefineProperties(cx, exports, mod->static_properties);
	ready = true;
}

Module::~Module()
{
	JSAutoRequest req(cx);
	JSAutoEnterCompartment comp;
	comp.enter(cx, obj);
	JS_SetContextThread(cx);
	JS_RemoveObjectRoot(cx, &obj);
	JS_ClearContextThread(cx);
}
