#pragma once

#ifndef __D2BS_H__

#ifdef _MSVC_DEBUG

#undef _DEBUG_NEW_REDEFINE_NEW
#define _DEBUG_NEW_REDEFINE_NEW 1
#undef _DEBUG_NEW_TAILCHECK
#define _DEBUG_NEW_TAILCHECK 4
#undef _DEBUG_NEW_PROGNAME
#define _DEBUG_NEW_PROGNAME "D2BS"

#else

#undef _DEBUG_NEW_REDEFINE_NEW
#define _DEBUG_NEW_REDEFINE_NEW 0
#undef _DEBUG_NEW_TAILCHECK
#define _DEBUG_NEW_TAILCHECK 0

struct Module
{	
	union {
		HMODULE hModule;
		DWORD dwBaseAddress;
	};
	DWORD _1;
	CHAR szPath[MAX_PATH];
};

#define new new (std::nothrow)

#endif

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved);

#endif
