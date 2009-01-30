#include "D2BS.h"

Control* findControl(DWORD dwType, DWORD dwX, DWORD dwY, DWORD dwSizeX, DWORD dwSizeY)
{
	for(Control* pControl = *p_D2WIN_FirstControl; pControl; pControl = pControl->pNext)
	{
		if(pControl->dwType == dwType || dwType == -1)
			if(pControl->dwPosX == dwX || dwX == -1)
				if(pControl->dwPosY == dwY || dwY == -1)
					if(pControl->dwSizeX == dwSizeX || dwSizeX == -1)
						if(pControl->dwSizeY == dwSizeY || dwSizeY == -1)
							return pControl;
	}

	return NULL;
}

void clickControl( Control * pControl )
{
	SendMouseClick(pControl->dwPosX + (pControl->dwSizeX / 2), pControl->dwPosY - (pControl->dwSizeY / 2), 0);
	SendMouseClick(pControl->dwPosX + (pControl->dwSizeX / 2), pControl->dwPosY - (pControl->dwSizeY / 2), 1);
}

BOOL OOG_SelectCharacter(char * szCharacter)
{
	// Select the first control on the character selection screen.
	Control* pControl = findControl(4,237,178,72,93);
	ControlText* cText;

	while (pControl != NULL)
	{
		if(pControl->pFirstText != NULL && pControl->pFirstText->pNext != NULL)
			cText = pControl->pFirstText->pNext;
		else
			cText = NULL;

		if (cText != NULL)
		{
			char * szLine = UnicodeToAnsi(cText->wText);
			if (strlen(szLine) == strlen(szCharacter) && strstr(szLine,szCharacter) != NULL)
			{
				clickControl(pControl);
				// OK Button
				pControl = findControl(6,627,572,128,35);
				clickControl(pControl);
				delete[] szLine;
				return TRUE;
			}
			delete[] szLine;
		}

		pControl = pControl->pNext;
	}
	return FALSE;
}