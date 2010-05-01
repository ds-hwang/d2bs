#pragma once
#ifndef __AREA_H__
#define __AREA_H__

#include <windows.h>

struct myArea {
	DWORD AreaId;
	DWORD Exits;
	// HACK: This should be just JSObject* but I don't want to hack in the js code here
	// we'll think of a better solution later.
	struct JSObject* ExitArray;
};

myArea* GetArea(__int32 area);
const char* GetAreaName(myArea* area);

#endif
