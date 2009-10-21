#include <windows.h>

#include "Offsets.h"
#include "Patch.h"

#include "debug_new.h"

#define INT3	0xCC
#define CALL	0xE8
#define NOP		0x90
#define JMP		0xE9
#define RET		0xC3

void InterceptLocalCode(PatchType type, DWORD addr, DWORD func, size_t len)
{
	static BYTE Instruction[] = {INT3, JMP, CALL, NOP, RET};
	BYTE *code = new BYTE[len];
	::memset(code, NOP, len);
	code[0] = Instruction[type];
	*(LPDWORD)&code[1] = func - (addr + 5);
	WriteBytes(addr, code, len);

	delete[] code;
}

bool ReadBytes(DWORD addr, void *data, size_t len)
{
	DWORD oldprot;

	if(!VirtualProtect((void*)addr, len, PAGE_READWRITE, &oldprot))
		return false;

	::memcpy(data, (void*)addr, len);
	return !!VirtualProtect((void*)addr, len, oldprot, &oldprot);
}

bool WriteBytes(DWORD addr, void *data, size_t len)
{
	DWORD oldprot;

	if(!VirtualProtect((void*)addr, len, PAGE_READWRITE, &oldprot))
		return false;

	::memcpy((void*)addr, data, len);
	return !!VirtualProtect((void*)addr, len, oldprot, &oldprot);
}

void ApplyPatch(Patch& p)
{
	if(p.OldValue != NULL) // if the patch is already applied, leave
		return;
	p.OldValue = new BYTE[p.Size];
	FillDllAddress(p.Place);
	ReadBytes(p.Place.FullAddress, p.OldValue, p.Size);
	InterceptLocalCode(p.Type, p.Place.FullAddress, p.NewValue, p.Size);
}

void RemovePatch(Patch& p)
{
	if(p.OldValue == NULL)
		return; // if the patch is not applied, leave
	WriteBytes(p.Place.FullAddress, p.OldValue, p.Size);
	delete[] p.OldValue;
}

void ApplyPatches(void)
{
	for(int i = 0; Patches[i].Type != NoPatch; i++)
		ApplyPatch(Patches[i]);
}

void RemovePatches(void)
{
	for(int i = 0; Patches[i].Type != NoPatch; i++)
		RemovePatch(Patches[i]);
}

void FillDllAddress(Offset& offset)
{
	offset.FullAddress = GetDllAddress(offset.DllName, offset.Address);
}

DWORD GetDllAddress(int dll, int Address)
{
	static char *dlls[] = {
		"D2Client.DLL",	"D2Common.DLL",	"D2Gfx.DLL",	"D2Lang.DLL", 
		"D2Win.DLL",	"D2Net.DLL",	"D2Game.DLL",	"D2Launch.DLL",
		"Fog.DLL",		"BNClient.DLL",	"Storm.DLL",	"D2Cmp.DLL",
		"D2Multi.DLL"
	};
	if(dll > -1 && dll < 13)
		return GetDllAddress(dlls[dll], Address);
	return 0;
}
DWORD GetDllAddress(const char *DllName, int Address)
{
	HMODULE hMod = GetModuleHandle(DllName);
	if(!hMod) hMod = LoadLibrary(DllName);
	if(!hMod) return 0;
	return (Address < 0 ? GetDllOrdinal(hMod, -Address) : GetDllOffset(hMod, Address));
}

DWORD GetDllOffset(HMODULE hMod, int Offset)
{
	return ((DWORD)hMod)+Offset;
}

DWORD GetDllOrdinal(HMODULE hMod, int Ordinal)
{
	return (DWORD)GetProcAddress(hMod, (LPCSTR)(Ordinal));
}
