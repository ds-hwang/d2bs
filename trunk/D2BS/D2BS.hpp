#pragma once

#define EXPORTED

#include "ScriptEngine.hpp"
#include "sqlite3.h"

extern ScriptEngine* engine;
extern HINSTANCE hModule;
extern char* base;
extern bool isGuarded;
extern sqlite3* settings;
