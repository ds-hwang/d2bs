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

class ScriptEngine
{
private:
	JSRuntime* runtime;
	std::string path;
	std::map<const char*, Script*> scripts;
	std::list<JSContext*> held, free;

	// NB: this behavior is roughly equivalent to these functions
	// "friend"ing ScriptEngine but looks just a little neater, IMO.
	static JSBool ContextCallback(JSContext *cx, uintN contextOp);
	static void ErrorReporter(JSContext *cx, const char *message, JSErrorReport *report);

public:
	EXPORT ScriptEngine(const char* path, unsigned int gctime);
	EXPORT ~ScriptEngine();

	JSContext* GetContext();
	void ReleaseContext(JSContext* cx);

	void InitClasses(Script* script);

	EXPORT Script* Compile(const char* file, bool recompile = false);
	EXPORT void Stop();
	EXPORT void FireEvent(const char* evtName, char* format, ...);
};
