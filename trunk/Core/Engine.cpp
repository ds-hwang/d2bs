#include "Engine.hpp"
#include <algorithm>
#include <iterator>
#include <assert.h>

#include "JSClasses.hpp"
#include "Modules.hpp"

using namespace std;

Engine::Engine(const wchar_t* path, unsigned int gctime,
						JSClassInitCallback classCallback,
						JSErrorReporter reporter) :
		path(L""), runtime(nullptr), classInit(classCallback), reporter(reporter)
{
	runtime = JS_NewRuntime(gctime);
	JS_SetRuntimePrivate(runtime, this);

	int len = wcslen(path);
	wchar_t* pname = new wchar_t[len+1];
	std::replace_copy(path, path+len, pname, L'/', L'\\');
	std::transform(pname, pname+len, pname, towlower);
	this->path = std::wstring(path);

	this->classInit = classCallback;

	JS_SetContextCallback(runtime, Engine::ContextCallback);

	JSContext* cx = GetContext();
	assert(cx != nullptr);
	ReleaseContext(cx);
}

Engine::~Engine(void)
{
	StopAll();
	auto mend = modules.end();
	for(auto it = modules.begin(); it != mend; it++)
		delete it->second;

	auto send = scripts.end();
	for(auto it = scripts.begin(); it != send; it++)
		delete it->second;

	assert(held.size() == 0);

	for(auto it = free.begin(); it != free.end(); it++)
		JS_DestroyContext(*it);

	JS_DestroyRuntime(runtime);
	JS_ShutDown();
}

JSContext* Engine::GetContext()
{
	JSContext* cx = nullptr;
	if(free.size() > 0)
	{
		cx = free.front();
		free.remove(cx);
		held.push_back(cx);
	}
	else
	{
		cx = JS_NewContext(runtime, 0x2000);
		held.push_back(cx);
	}

	JS_SetContextThread(cx);
	return cx;
}
void Engine::ReleaseContext(JSContext* cx)
{
	assert(std::find(held.begin(), held.end(), cx) != held.end());
	JS_SetContextPrivate(cx, nullptr);
	JS_ClearContextThread(cx);
	held.remove(cx);
	free.push_back(cx);
}

void Engine::RunAll(void) { for(auto it = scripts.begin(); it != scripts.end(); it++) it->second->Start(); }
void Engine::StopAll(void) { for(auto it = scripts.begin(); it != scripts.end(); it++) it->second->Pause(); }
void Engine::PauseAll(void) { for(auto it = scripts.begin(); it != scripts.end(); it++) it->second->Stop(); }

bool Engine::Exists(const wchar_t* rel, bool module)
{
	if(module) return modules.count(rel) > 0;
	else return _waccess((this->path + L"\\" + rel).c_str(), 0) != -1;
}

Script* Engine::CompileScript(const wchar_t* file, bool recompile)
{
	// normalize the file name
	std::wstring fname(file);
	std::replace(fname.begin(), fname.end(), L'/', L'\\');
	std::transform(fname.begin(), fname.end(), fname.begin(), towlower);
	const wchar_t* scriptName = fname.c_str();

	bool exists = scripts.count(scriptName) > 0;
	if(!recompile && exists) return scripts[scriptName];
	else if(recompile && exists) {
		Script* script = scripts[scriptName];
		script->Stop();
		scripts.erase(scriptName);
		delete script;
	}

	std::wstring fullpath = path + L"\\" + fname;
	Script* script = new Script(fullpath.c_str(), this);
	scripts[scriptName] = script;

	return script;
}

Module* Engine::CompileModule(JSContext* cx, const wchar_t* file)
{
	// normalize the file name
	std::wstring fname(file);
	std::replace(fname.begin(), fname.end(), L'/', L'\\');
	std::transform(fname.begin(), fname.end(), fname.begin(), towlower);
	const wchar_t* moduleName = fname.c_str();

	bool exists = modules.count(moduleName) > 0;
	if(exists) return modules[moduleName];

	std::wstring fullpath = path + L"\\" + fname;
	Module* module = new Module(cx, fullpath.c_str(), this);
	modules[moduleName] = module;

	return module;
}

void Engine::RegisterModule(JSModuleSpec* modlist) { mods.push_back(modlist); }
void Engine::InitClasses(Script* script) { InitClasses(script->cx, script->obj); }
void Engine::InitClasses(JSContext* cx, JSObject* obj)
{
	JS_InitStandardClasses(cx, obj);
	JS_DefineFunctions(cx, obj, module_methods);

	if(classInit != nullptr) classInit(cx, obj);
	InitModules(cx, obj);
}

void Engine::InitModules(JSContext* cx, JSObject* obj)
{
	JSAutoRequest req(cx);
	JSAutoEnterCompartment comp;
	comp.enter(cx, obj);

	JSObject* gobj = JS_NewGlobalObject(cx, &global);
	JS_InitStandardClasses(cx, gobj);
	auto end = mods.end();
	for(auto it = mods.begin(); it != end; it++)
	{
		for(JSModuleSpec* mod = *it; mod->name != nullptr; mod++)
		{
			Module* m = new Module(cx, gobj, mod);
			this->modules[mod->name] = m;
		}
	}
}

void Engine::FireEvent(const char* evtName, char* format, ...)
{
}

JSBool Engine::ContextCallback(JSContext *cx, uintN contextOp)
{
	if(contextOp != JSCONTEXT_NEW) return JS_TRUE;

	// set options, register handlers
#ifdef _DEBUG
	//JS_SetGCZeal(cx, 1);
#endif
	JS_SetOperationCallback(cx, Script::OperationCallback);
	JS_SetErrorReporter(cx, Engine::ErrorReporter);
	JS_SetVersion(cx, JSVERSION_LATEST);
	JS_SetOptions(cx, JSOPTION_JIT|
					  JSOPTION_METHODJIT|
					  JSOPTION_RELIMIT|
					  JSOPTION_VAROBJFIX|
					  JSOPTION_XML|
					  JSOPTION_STRICT|
					  JSOPTION_ANONFUNFIX);

	return JS_TRUE;
}

void Engine::ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	Engine* engine = (Engine*)JS_GetRuntimePrivate(JS_GetRuntime(cx));
	if(engine->reporter != nullptr) engine->reporter(cx, message, report);
}
