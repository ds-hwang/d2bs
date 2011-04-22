#pragma once
#define XP_WIN

#include "jsapi.h"
#include <windows.h>
#include <string>
#include <vector>

#include "Script.h"

class ScriptEngine
{
private:
	JSRuntime* runtime;
	std::string path;
	std::vector<Script*> scripts;

public:
	ScriptEngine(std::string path);
	Script* Compile(std::string path, bool autostart = true, bool recompile = false);
	void FireEvent(std::string evtName, char* format, ...);
};
