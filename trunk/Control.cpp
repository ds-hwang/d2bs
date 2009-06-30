//#include "D2BS.h"
#include "Control.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"

#include "debugnew/debug_new.h"

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

bool clickControl(Control* pControl, int x, int y)
{
	if(pControl)
	{
		if(x == -1)
			x = pControl->dwPosX + (pControl->dwSizeX / 2);
		if(y == -1)
			y = pControl->dwPosY - (pControl->dwSizeY / 2);
		SendMouseClick(x, y, 0);
		Sleep(150);
		SendMouseClick(x, y, 1);
		return true;
	}
	return false;
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

void SetControlText(Control* control, char* text)
{
	if(control && text)
	{
		wchar_t* szwText = AnsiToUnicode(text);
		D2WIN_SetControlText(control, szwText);
		delete[] szwText;
	}
}