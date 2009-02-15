#pragma once

#ifndef __SYNCLOCK_H__
#define __SYNCLOCK_H__

#include <windows.h>

// oh god, this is such an ugly abuse of the preprocessor :(

#define LOCK(name) class name { \
	static CRITICAL_SECTION SyncObj; \
public: \
	name##() { EnterCriticalSection(&SyncObj); } \
	~##name##() { LeaveCriticalSection(&SyncObj); } \
	static void Initialize(void) { InitializeCriticalSection(&SyncObj); } \
	static void Destroy(void) { DeleteCriticalSection(&SyncObj); } \
}; \
CRITICAL_SECTION name##::SyncObj = {0};

LOCK(PrintLock)


#endif
