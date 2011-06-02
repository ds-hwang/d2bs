#pragma once

#define EXPORTED

#include "Engine.hpp"
#include "sqlite3.h"

extern Engine* engine;
extern HINSTANCE hModule;
extern char* base;
extern bool isGuarded;
extern sqlite3* settings;
