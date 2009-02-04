#pragma once

class CDebug
{
	private:
	CHAR szString[64];

	public:
	CDebug(CHAR* pString)
	{
		strcpy(szString, pString);
		if(Vars.bDebug)
			Log("DEBUG: Entering function '%s'", szString);
	}

	~CDebug()
	{
		if(Vars.bDebug)
			Log("DEBUG: Leaving function '%s'", szString);
	}
};
