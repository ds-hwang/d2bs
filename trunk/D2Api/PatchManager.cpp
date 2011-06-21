#include "PatchManager.hpp"

bool PatchManager::InstallPatches(void)
{
	for(int x = 0; x < arraySize(pHooks); x++)
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

	}
	return true;
}

void PatchManager::RemovePatches()
{
	for(int x = 0; x < arraySize(pHooks); x++) {
		if(!WriteBytes((void*)pHooks[x].dwAddr, pHooks[x].bOldCode, pHooks[x].dwLen)) {
			delete[] pHooks[x].bOldCode;
			throw;
		}
		delete[] pHooks[x].bOldCode;
	}	
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
