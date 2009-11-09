#include "SMEngine.h"

#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) SMEngine* __fastcall CreateEngine(void)
{
	return new SMEngine();
}

#ifdef __cplusplus
}
#endif
