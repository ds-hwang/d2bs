#pragma once
#include <windows.h>

#include <string>
#include <list>
#include <vector>
#include <unordered_map>

#include "js32.hpp"
#define EXPORTING
#include "Exports.hpp"
#undef EXPORTING

namespace Core {
class Engine;
}

#include "Script.hpp"
#include "Module.hpp"
#include "Event.hpp"

namespace Core {

typedef void (*JSClassInitCallback)(JSContext* cx, JSObject* obj);

class Engine
{
private:
	Engine(const Engine&);
	Engine& operator=(const Engine&);

	JSRuntime* runtime;
	std::wstring path;
	std::unordered_map<std::wstring, Script*> scripts;
	std::unordered_map<std::wstring, Module*> modules;
	std::list<JSContext*> held, free;
	std::list<JSModuleSpec*> mods;
	JSErrorReporter reporter;
	JSClassInitCallback classInit;

	void InitModules(JSContext* cx, JSObject* obj);

	// NB: this behavior is roughly equivalent to these functions
	// "friend"ing Engine but looks just a little neater, IMO.
	static JSBool ContextCallback(JSContext *cx, uintN contextOp);
	static void ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report);

public:
	EXPORT Engine(const wchar_t* path, unsigned int gctime,
						JSClassInitCallback classCallback = nullptr,
						JSErrorReporter reporter = nullptr);
	EXPORT ~Engine(void);

	EXPORT JSContext* GetContext();
	EXPORT void ReleaseContext(JSContext* cx);

	EXPORT inline const wchar_t* GetPath(void) { return path.c_str(); }
	EXPORT inline JSRuntime* GetRuntime(void) { return runtime; }

	EXPORT void RunAll(void);
	EXPORT void StopAll(void);
	EXPORT void PauseAll(void);

	EXPORT void InitClasses(Script* script);
	EXPORT void InitClasses(JSContext* cx, JSObject* obj);
	EXPORT void RegisterModule(JSModuleSpec* module);

	EXPORT inline bool FileExists(const wchar_t* rel) { return _waccess((this->path + L"\\" + rel).c_str(), 0) != -1; }
	EXPORT inline bool ModuleExists(const wchar_t* rel) { return modules.count(rel) > 0; }

	EXPORT Script* CompileScript(const wchar_t* file, bool recompile = false);
	EXPORT Module* CompileModule(JSContext* cx, const wchar_t* module);

	EXPORT void FireEvent(Event* evt);
};

}
