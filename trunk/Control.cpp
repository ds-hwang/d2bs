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
	return TRUE;//testing only -- remove after testing!

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

BOOL OOG_SelectGateway(char * szGateway)
{
	if(strstr(szGateway,"ERROR"))
		return FALSE;
	// Select the gateway control.
	Control* pControl = findControl(6,264,391,272,25);

	// if the control exists and has the text label, check if it matches the selected gateway
	if(pControl != NULL && pControl->wText2 != NULL)
	{
		char * szLine = _strlwr(UnicodeToAnsi(pControl->wText2));
		szGateway = _strlwr(szGateway);
		
		if(strstr(szLine,szGateway) != NULL)
		{
		// gateway is correct, do nothing and return true
			delete[] szLine;
			return TRUE;
		}
		else
		{
			// gateway is NOT correct, change gateway to selected gateway if it exists
			clickControl(pControl); // open the gateway select screen
			int index = 0;
			bool gatefound = false;

			// loop here till we find the right gateway if we can
			pControl = findControl(4,257,500,292,160);
			ControlText* cText;
			if(pControl != NULL && pControl->pFirstText != NULL)
			{
				cText = pControl->pFirstText;
				while(cText != NULL)
				{
					char * szGatelist = _strlwr(UnicodeToAnsi(cText->wText));
					if(strstr(szGatelist,szGateway) != NULL)
					{// chosen gateway IS in the list and matches, cleanup and break the loop
						gatefound = true;
						delete[] szGatelist;
						break;
					}
					delete[] szGatelist;
					index++;
					cText = cText->pNext;
				}
				if(gatefound)
				{
					/*cText = pControl->pSelectedText;
					while(cText != NULL)
					{
						char * szGatelist = _strlwr(UnicodeToAnsi(cText->wText));
						if(strstr(szGatelist,szGateway) != NULL)
							break;
						delete[] szGatelist;
						cText = cText->pNext;
						Sleep(1500);
					}*/
				// click the correct gateway using the control plus a default x and a y based on (index*24)+12
				clickControl(pControl,-1,pControl->dwPosY + ((index*24)+12));
				}
			}

			// OK Button, gateway select screen
			pControl = findControl(6,281,538,96,32);
			//clickControl(pControl);
			delete[] szLine;
			return TRUE;
		}
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