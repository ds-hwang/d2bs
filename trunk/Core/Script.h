#pragma once

#define EXPORTING
#define XP_WIN

#include "jsapi.h"
#include <windows.h>
#include "Exports.hpp"

class EXPORT Script
{
	Script(const Script&);
	Script& operator=(const Script&);

public:
	Script(const char* path);
};
