#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <Windows.h>

#include "D2Api.hpp"

//#include "D2Structs.h"

using namespace std;

struct D2DllInfo {
	char* DllName;
	D2DllNo DllNo;
};

const D2DllInfo dlls[] = {
	{ "D2Client.DLL", D2CLIENT }, 
	{ "D2Common.DLL", D2COMMON },
	{ "D2Gfx.DLL", D2GFX },
	{ "D2Lang.DLL", D2LANG },
	{ "D2Win.DLL", D2WIN },
	{ "D2Net.DLL", D2NET },
	{ "D2Game.DLL", D2GAME },
	{ "D2Launch.DLL", D2LAUNCH },
	{ "Fog.DLL", FOG },
	{ "BNClient.DLL", BNCLIENT },
	{ "Storm.DLL", STORM },
	{ "D2Cmp.DLL", D2CMP },
	{ "D2Multi.DLL", D2MULTI}
};

struct ModuleInfo {
	string ModuleName;
	int ModuleNo;
	DWORD BaseAddress;
};

struct Offset {
	ModuleInfo* Module;
	DWORD BaseOffset;
};

typedef vector<Offset*> Offsets;
typedef vector<ModuleInfo*> Modules;

template<typename Type, size_t Size>
char (&ArraySizeHelper(Type(&Array)[Size]))[Size];
#define arraySize(Array) sizeof(ArraySizeHelper(Array))

class PointerManager {
private:
	static PointerManager* instance;

	Offsets offsetList;
	Modules moduleList;
	bool basesDefined;
	bool offsetsDefined;

	//ModuleInfo* GetModuleInfo(int num);

	template<typename S>
	S & InitPtr(S & Symbol, int Dllno, int Offset) {
		Symbol = reinterpret_cast<S>(GetDllOffset(Dllno, Offset));
		return Symbol;
	}
	template<>
	unsigned int & InitPtr<unsigned int>(unsigned int & Symbol, int Dllno, int Offset) {
		Symbol = GetDllOffset(Dllno, Offset);
		return Symbol;
	}

	PointerManager() : basesDefined(false), offsetsDefined(false) {}
	~PointerManager() { CleanUp(); }
	PointerManager(const PointerManager &);
	PointerManager & operator= (PointerManager const &);

public:
	static PointerManager * Instance();
	unsigned GetDllOffset(int DllNo, int Offset);

	bool DefineOffsets();
	bool DefineBases();

	void CleanUp();
};
