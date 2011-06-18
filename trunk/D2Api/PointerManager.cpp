#include <windows.h>
#include <cstdio>

//#include "D2Structs.h"
#include "PointerManager.hpp"
//#include "Patch.h"

using namespace std;

namespace D2Client {
	/* D2Client Functions */
	//PrintGameString_t PrintGameString;

	/* D2Client Vars */
	//UnitAny** PlayerUnit;

	/* Assembly Routines */
	//unsigned GetUnitFromId_I;
}

namespace D2Win {
	/* D2Win Functions */

	/* D2Win Vars */
}

namespace D2Gfx {
	/* D2Gfx Functions */
}

namespace D2Common {
	/* D2Common Functions */
}

namespace D2Launch {
	/* D2Launch Vars*/
}

namespace D2Net {
	/* D2Net Functions */
}

namespace D2Cmp {
	/* D2Cmp Functions */
}

namespace D2Lang {\
	/* D2Lang Functions */
}

namespace Fog {

}

namespace Storm {

}

PointerManager* PointerManager::instance = NULL;

PointerManager* PointerManager::Instance()
{
	return instance ? instance : ((instance = new PointerManager));
}

unsigned PointerManager::GetDllOffset(int DllNo, int Offset)
{
	HMODULE hMod = GetModuleHandle(moduleList[DllNo]->ModuleName.c_str());
	if(!hMod)
		hMod = LoadLibrary(moduleList[DllNo]->ModuleName.c_str());

	if(!hMod)
		throw;

	DWORD Addr = 0;
	if(Offset < 0) {
		Addr = (DWORD)GetProcAddress(hMod, (LPCSTR)(-Offset));
		printf("Addr %#08X for %s Ordinal %lu\n", Addr, moduleList[DllNo]->ModuleName.c_str(), -Offset);
	} else {
		Addr = ((DWORD)hMod)+Offset;
		printf("Addr %#08X for %s Offset %#08X\n", Addr, moduleList[DllNo]->ModuleName.c_str(), Offset);
	}

	if(Addr == 0)
		throw;

	return Addr;
}

void PointerManager::CleanUp() {
	if(moduleList.size()) {
		for(unsigned i = 0; i < moduleList.size(); i++) {
			delete moduleList[i];
		}
	}
}

bool PointerManager::DefineBases() {
	if(basesDefined)
		return false;

	if(moduleList.size())
		throw;

	int moduleCount = sizeof(dlls) / sizeof(dlls[0]); 
	for(int i = 0; i < moduleCount; i++) {
		ModuleInfo* modInfo = new ModuleInfo;
		modInfo->ModuleName = dlls[i].DllName;
		modInfo->ModuleNo = dlls[i].DllNo;
		modInfo->BaseAddress = 0;
		moduleList.push_back(modInfo);

		unsigned BaseAddress = GetDllOffset(i, 0);
		moduleList[i]->BaseAddress = BaseAddress;
	}

	basesDefined = true;

	return true;
}

bool PointerManager::DefineOffsets()
{
	try
	{
		if(!DefineBases())
			throw;
	}
	catch(const exception &ex)
	{
		throw(ex);
	}

	//InitPtr(D2Client::RevealAutomapRoom, D2CLIENT, 0x62580);

	return true;
}

bool PatchManager::InstallPatches(void) {
	/*PatchHook* pHooks = RetrievePatchHooks(nullptr);
	for(int x = 0; x < ArraySize(pHooks); x++)
	{
		DWORD Offset = PointerManager::Instance()->GetDllOffset(pHooks[x].dwDllNo, pHooks[x].dwAddr);
		pHooks[x].dwAddr = Offset;
		pHooks[x].bOldCode = new BYTE[pHooks[x].dwLen];
		if(::ReadProcessMemory(GetCurrentProcess(), (void*)Offset, pHooks[x].bOldCode, pHooks[x].dwLen, NULL) == 0) {
			printf("ReadProcessMemory: Offset='%#08X' Err'%d'\n", Offset, GetLastError());
			return false;
		}

		// Need better handling
		if(pHooks[x].Type == CALL)
			PatchCall(Offset, pHooks[x].dwFunc, pHooks[x].dwLen);
		else if(pHooks[x].Type == JMP)
			PatchJmp(Offset, pHooks[x].dwFunc, pHooks[x].dwLen);

	}*/
	return true;
}

void PatchManager::RemovePatches()
{
	/*PatchHook* pHooks = RetrievePatchHooks(nullptr);
	for(int x = 0; x < ArraySize(pHooks); x++) {
		if(!WriteBytes((void*)pHooks[x].dwAddr, pHooks[x].bOldCode, pHooks[x].dwLen)) {
			delete[] pHooks[x].bOldCode;
			throw;
		}
		delete[] pHooks[x].bOldCode;
	}*/	
}

bool PatchManager::WriteBytes(void *pAddr, void *pData, DWORD dwLen)
{
	DWORD dwOld;

	if(!VirtualProtect(pAddr, dwLen, PAGE_READWRITE, &dwOld))
		return false;

	::memcpy(pAddr, pData, dwLen);
	return VirtualProtect(pAddr, dwLen, dwOld, &dwOld) != FALSE;
}

bool PatchManager::FillBytes(void *pAddr, BYTE bFill, DWORD dwLen)
{
	BYTE *bCode = new BYTE[dwLen];
	::memset(bCode, bFill, dwLen);

	if(!WriteBytes(pAddr, bCode, dwLen)) {
		delete [] bCode;
		return false;
	}

	delete[] bCode;
	return true;
}

bool PatchManager::InterceptLocalCode(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen)
{
	BYTE *bCode = new BYTE[dwLen];
	::memset(bCode, 0x90, dwLen);
	DWORD dwFunc = pFunc - (pAddr + 5);

	bCode[0] = bInst;
	*(DWORD *)&bCode[1] = dwFunc;
	if(!WriteBytes((void*)pAddr, bCode, dwLen)) {
		delete [] bCode;
		return false;
	}

	delete [] bCode;
	return true;
}

void PatchManager::PatchCall(DWORD dwAddr, DWORD dwFunc, DWORD dwLen)
{
	if(!InterceptLocalCode(INST_CALL, dwAddr, dwFunc, dwLen))
		throw;
}

void PatchManager::PatchJmp(DWORD dwAddr, DWORD dwFunc, DWORD dwLen)
{
	if(!InterceptLocalCode(INST_JMP, dwAddr, dwFunc, dwLen))
		throw;
}

void PatchManager::PatchBytes(DWORD dwAddr, DWORD dwValue, DWORD dwLen)
{
	BYTE *bCode = new BYTE[dwLen];
	::memset(bCode, (BYTE)dwValue, dwLen);

	if(!WriteBytes((LPVOID)dwAddr, bCode, dwLen)) {
		delete [] bCode;
		throw;
	}

	delete [] bCode;
}

PatchHook *PatchManager::RetrievePatchHooks(PINT pBuffer)
{
	//*pBuffer = ArraySize(pHooks);
	return &pHooks[0];
}
