#pragma once
#define XP_WIN

#define EXPORTING

#include "Exports.hpp"
#include "js32.hpp"
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>

class Engine;

#include "Script.hpp"
#include "Module.hpp"

typedef void (*JSClassInitCallback)(JSContext* cx, JSObject* obj);

class Engine
{
private:
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

	// NB: the difference between a Module and a Script is that a Module may not be
	// recompiled, and will always return the same object for the same relative name
	EXPORT bool Exists(const wchar_t* rel, bool module = false);
	EXPORT Script* CompileScript(const wchar_t* file, bool recompile = false);
	EXPORT Module* CompileModule(JSContext* cx, const wchar_t* module);

	EXPORT void FireEvent(const char* evtName, char* format, ...);
};
