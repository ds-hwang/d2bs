#include <windows.h>
#include <cstdio>

//#include "D2Structs.h"
#include "D2Api.hpp"
#include "PointerManager.hpp"

using namespace std;

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

	int moduleCount = arraySize(dlls); 
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

	return true;
}
