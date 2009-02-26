// This file is for debugging purposes only.
// You must #define _MSVC_DEBUG in order to enable it.
// It enables loading from D2Loader as a plugin,
// making debugging easier.

#ifdef _MSVC_DEBUG

#include <windows.h>
#include "D2Utilities.h"

#define REASON_NONE					0x00
#define REASON_INIT					0x01
#define REASON_CLEANUP				0x02
#define REASON_ENTER_GAME			0x04
#define REASON_LEAVE_GAME			0x08
#define REASON_ENTER_CHANNEL		0x10
#define REASON_LEAVE_CHANNEL		0x20
#define REASON_ENTER_MAIN_MENU		0x40
#define REASON_LEAVE_MAIN_MENU		0x80

typedef DWORD ( __stdcall * PluginEntryFunc)(DWORD dwReason, LPVOID lpData);

DWORD __stdcall PluginEntry(DWORD dwReason, LPVOID lpData)
{
	if (dwReason == REASON_INIT)
	{

	}
	if (dwReason == REASON_CLEANUP)
	{
		D2BSCleanup();
	}

	return TRUE; 
}

typedef struct { 
	DWORD dwMagicword; 
	DWORD dwVersion;
	LPCSTR szDescription; 
	PluginEntryFunc fpEntry; 
} PLUGIN_INTERFACE, * LPPLUGIN_INTERFACE;

PLUGIN_INTERFACE Interface = { 0x44320000, 0x01000912, "D2BS 2.0", PluginEntry };

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllexport) LPPLUGIN_INTERFACE __cdecl QueryInterface(void) {
	return &Interface; 
}
#ifdef __cplusplus
}
#endif

#endif
