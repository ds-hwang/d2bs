#include "jsutilities.h"


JSBool InitContext(JSContext* cx, uintN contextOp)
{
	if(contextOp == JSCONTEXT_NEW)
	{
		// TODO: Add d2bs-specific context initializations
	}
	return JS_TRUE;
}
