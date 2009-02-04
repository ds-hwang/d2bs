#include "D2BS.h"
#include "Script.h"

#include "debugnew/debug_new.h"

DWORD DdeSrvInst = 0;
HSZ hszD2BSns;

HDDEDATA CALLBACK DdeCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, 
    HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2)
{
	char pszItem[65535];
	switch(uType) {
		case XTYP_CONNECT:
			return (HDDEDATA)TRUE;
		case XTYP_POKE:
		case XTYP_EXECUTE:
			DdeGetData(hdata, (LPBYTE)pszItem, sizeof(pszItem), 0);
			Script* script = Script::CompileCommand(pszItem);
			if(script)
				CreateThread(0, 0, ScriptThread, script, 0, 0);
			break;
	}
	return (HDDEDATA)0;
}

DWORD CreateDdeServer() {
	char buf[1000];

	int ret = DdeInitialize(&DdeSrvInst, DdeCallback,
				APPCLASS_STANDARD | APPCMD_FILTERINITS | CBF_FAIL_ADVISES |
				CBF_FAIL_REQUESTS | CBF_SKIP_CONNECT_CONFIRMS | CBF_SKIP_REGISTRATIONS |
				CBF_SKIP_UNREGISTRATIONS, 0);
	if(ret != DMLERR_NO_ERROR)
		return 0;
	char handle[25];
	sprintf(handle, "d2bs-%d", GetProcessId(GetCurrentProcess()));
	hszD2BSns = DdeCreateStringHandle(DdeSrvInst, handle, CP_WINANSI);
	if(!hszD2BSns)
		return 0;
	if(!DdeNameService(DdeSrvInst, hszD2BSns, 0L, DNS_REGISTER | DNS_FILTERON)) {
		sprintf(buf, "DdeServer DdeNameService Error: %X", DdeGetLastError(DdeSrvInst));
		OutputDebugString(buf);
		return 0;
	}
	return GetLastError();
}

BOOL ShutdownDdeServer() {
	DdeFreeStringHandle(DdeSrvInst, hszD2BSns);
	return DdeUninitialize(DdeSrvInst);
}
