#pragma once
#include <windows.h>

#define EXPORTING
#include "..\Core\Exports.hpp"
#undef EXPORTING

extern "C" {
	EXPORT void __fastcall Init(void);
}
