#pragma once

#include <cstring>
#include "D2Helpers.h"
#include "D2BS.h"

class CDebug
{
	private:
	char szString[64];

	public:
	CDebug(char* pString)
	{
		strcpy_s(szString, sizeof(szString), pString);
		if(Vars.bDebug)
			Log("DEBUG: Entering function '%s'", szString);
	}

	~CDebug()
	{
		if(Vars.bDebug)
			Log("DEBUG: Leaving function '%s'", szString);
	}
};
