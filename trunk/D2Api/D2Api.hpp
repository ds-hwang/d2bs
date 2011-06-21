#pragma once

#include <Windows.h>

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

extern "C" {
	EXPORT void __fastcall Init(void);
}

