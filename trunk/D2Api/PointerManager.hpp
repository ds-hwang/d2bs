#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <Windows.h>

//#include "D2Structs.h"

using namespace std;

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
