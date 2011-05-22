#pragma once
#define XP_WIN

#define EXPORTING

#include "Exports.hpp"
#include "js32.hpp"
#include <windows.h>
#include <string>
#include <list>
#include <map>

class ScriptEngine;

#include "Script.hpp"

typedef void (*JSClassInitCallback)(Script* script);

class ScriptEngine
{
private:
	JSRuntime* runtime;
	std::wstring path;
	std::map<const wchar_t*, Script*> scripts;
	std::list<JSContext*> held, free;
	static JSErrorReporter reporter;
	JSClassInitCallback classInit;

	// NB: this behavior is roughly equivalent to these functions
	// "friend"ing ScriptEngine but looks just a little neater, IMO.
	static JSBool ContextCallback(JSContext *cx, uintN contextOp);
	static void ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report);

public:
	EXPORT ScriptEngine::ScriptEngine(const wchar_t* path, unsigned int gctime,
						JSClassInitCallback classCallback = 0);
	EXPORT ~ScriptEngine();

	EXPORT JSContext* GetContext();
	EXPORT void ReleaseContext(JSContext* cx);

	EXPORT inline const wchar_t* GetPath(void) { return path.c_str(); }
	EXPORT inline bool ExistsInPath(const wchar_t* rel) {
		std::wstring path = this->path + rel;
		return _waccess(path.c_str(), 0) != -1;
	}
	EXPORT inline JSRuntime* GetRuntime(void) { return runtime; }

	// NB: The engine can only support one reporter, since I can't
	// figure out how to pass the engine to the error reporter :(
	// Eventually I'd like to be able to support as many error reporters
	// as there are engine instances
	static void SetErrorReporter(JSErrorReporter reporter);

	void InitClasses(Script* script);

	EXPORT Script* Compile(const wchar_t* file, bool recompile = false);
	EXPORT void Stop();
	EXPORT void FireEvent(const char* evtName, char* format, ...);
};
