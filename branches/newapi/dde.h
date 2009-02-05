#pragma once
#include <windows.h>

HDDEDATA	CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2);
DWORD		CreateDdeServer();
BOOL		ShutdownDdeServer();
