#pragma once

//#include <Windows.h>

#include "Exports.hpp"

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
