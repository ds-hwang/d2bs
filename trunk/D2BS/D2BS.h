#pragma once
#include "ScriptEngine.h"
#include "sqlite3.h"

extern ScriptEngine* engine;
extern HINSTANCE hModule;
extern char* base;
extern bool isGuarded;
extern sqlite3* settings;
