#include "V8Engine.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) V8Engine* __fastcall CreateEngine(void)
{
	return new V8Engine();
}

#ifdef __cplusplus
}
#endif
