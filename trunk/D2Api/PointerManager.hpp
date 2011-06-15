#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <windows.h>

//#include "D2Structs.h"

using namespace std;

enum D2DllNo {
	D2CLIENT, 
	D2COMMON, 
	D2GFX, 
	D2LANG, 
	D2WIN, 
	D2NET, 
	D2GAME, 
	D2LAUNCH, 
	FOG, 
	BNCLIENT, 
	STORM, 
	D2CMP, 
	D2MULTI
};

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

namespace D2Client {
	typedef void (__stdcall *PrintGameString_t)(wchar_t *wMessage, int nColor);

	extern PrintGameString_t PrintGameString;
}

namespace D2Win {
}

namespace D2Gfx {
}

namespace D2Common {
}

namespace D2Launch {
}

namespace D2Net {
}

namespace D2Cmp {
}

namespace D2Lang {
}

namespace Fog {
}

namespace Storm {
}

template<class T, ptrdiff_t n >
ptrdiff_t arraySize(T(&)[n]) { return n; }

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

#define INST_INT3	0xCC
#define INST_CALL	0xE8
#define INST_NOP	0x90
#define INST_JMP	0xE9
#define INST_RET	0xC3

enum PatchType {
	CALL,
	JMP
};

typedef struct PatchHook_t
{
	PatchType Type;
	DWORD dwDllNo;
	DWORD dwAddr;
	DWORD dwFunc;
	DWORD dwLen;
	BYTE *bOldCode;
} PatchHook;

class PatchManager {
private:
	PatchHook* pHooks;

	PatchHook *RetrievePatchHooks(PINT pBuffer);
	void PatchBytes(DWORD dwAddr, DWORD dwValue, DWORD dwLen);
	void PatchJmp(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
	void PatchCall(DWORD dwAddr, DWORD dwFunc, DWORD dwLen);
	bool InterceptLocalCode(BYTE bInst, DWORD pAddr, DWORD pFunc, DWORD dwLen);
	bool FillBytes(void *pAddr, BYTE bFill, DWORD dwLen);
	bool WriteBytes(void *pAddr, void *pData, DWORD dwLen);

public:
	PatchManager() : pHooks(nullptr) {}
	~PatchManager() {}

	void RemovePatches();
	bool InstallPatches();
};
