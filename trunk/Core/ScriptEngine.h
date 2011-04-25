#pragma once
#define XP_WIN

#include "jsapi.h"
#include <windows.h>
#include <string>
#include <map>

#include "Script.h"
#include "Exports.hpp"

class ScriptEngine
{
private:
	JSRuntime* runtime;
	std::string path;
	std::map<const char*, Script*> scripts;

public:
	EXPORT ScriptEngine(const char* path, unsigned int gctime);
	EXPORT ~ScriptEngine();

	EXPORT Script* Compile(const char* file, bool recompile = false);
	EXPORT void FireEvent(const char* evtName, char* format, ...);
};
