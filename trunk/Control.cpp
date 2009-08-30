#include "Control.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"

#include "debugnew/debug_new.h"

Control* findControl(DWORD dwType, char* Text, DWORD dwDisabled, DWORD dwPosX, DWORD dwPosY, DWORD dwSizeX, DWORD dwSizeY)
{
	BOOL bFound = FALSE;

	for(Control* pControl = *p_D2WIN_FirstControl; pControl; pControl = pControl->pNext)
	{
		if(dwType >= 0 && pControl->dwType == dwType)
			bFound = TRUE;
		else if(dwType >= 0 && pControl->dwType != dwType)
		{
			bFound = FALSE;
			continue;
		}

		if(Text && pControl->dwType == 6)
		{
			if(strcmp(UnicodeToAnsi(pControl->wText2), Text) == 0)
				bFound = TRUE;
			else {
				bFound = FALSE;
				continue;
			}
		}

		if(dwDisabled >= 0 && pControl->dwDisabled == dwDisabled)
		{
			if(pControl->dwType == 6 && pControl->unkState == 1)
			{
				bFound = FALSE;
				continue;
			}
			bFound = TRUE;
		}
		else if(dwDisabled >= 0 && pControl->dwDisabled != dwDisabled)
		{			
			bFound = FALSE;
			continue;
		}

		if(dwPosX >= 0 && pControl->dwPosX == dwPosX)
			bFound = TRUE;
		else if(dwPosX >= 0 && pControl->dwPosX != dwPosX)
		{
			bFound = FALSE;
			continue;
		}

		if(dwPosY >= 0 && pControl->dwPosY == dwPosY)
			bFound = TRUE;
		else if(dwPosY >= 0 && pControl->dwPosY != dwPosY)
		{
			bFound = FALSE;
			continue;
		}

		if(dwSizeX >= 0 && pControl->dwSizeX == dwSizeX)
			bFound = TRUE;
		else if(dwSizeX >= 0 && pControl->dwSizeX != dwSizeX)
		{
			bFound = FALSE;
			continue;
		}

		if(dwSizeY >= 0 && pControl->dwSizeY == dwSizeY)
			bFound = TRUE;
		else if(dwSizeY >= 0 && pControl->dwSizeY != dwSizeY)
		{
			bFound = FALSE;
			continue;
		}

		if(bFound)
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
	Control* pControl = findControl(4, NULL, -1, 237, 178, 72, 93);
	ControlText* cText;

	while(pControl != NULL)
	{
		if(pControl->pFirstText != NULL && pControl->pFirstText->pNext != NULL)
			cText = pControl->pFirstText->pNext;
		else
			cText = NULL;

		if(cText != NULL)
		{
			char * szLine = UnicodeToAnsi(cText->wText);
			if (strlen(szLine) == strlen(szCharacter) && strstr(szLine,szCharacter) != NULL)
			{
				if(!clickControl(pControl))
					return FALSE;

				// OK Button
				if(pControl = findControl(6, NULL, -1, 627, 572, 128, 35))
				{
					if(!clickControl(pControl))
					{
						delete[] szLine;
						return FALSE;
					}
				
					delete[] szLine;
					return TRUE;
				}
				else
				{
					delete[] szLine;
					return FALSE;
				}

			}
			delete[] szLine;
		}
		pControl = pControl->pNext;
	}
	return FALSE;
}

void SetControlText(Control* pControl, char* Text)
{
	if(pControl && Text)
	{
		wchar_t* szwText = AnsiToUnicode(Text);
		D2WIN_SetControlText(pControl, szwText);
		delete[] szwText;
	}
}

BOOL OOG_SelectGateway(char * szGateway)
{
	if(strstr(szGateway,"ERROR"))
		return FALSE;
	// Select the gateway control.
	Control* pControl = findControl(6, NULL, -1, 264, 391, 272, 25);

	// if the control exists and has the text label, check if it matches the selected gateway
	if(pControl && pControl->wText2)
	{
		char * szLine = _strlwr(UnicodeToAnsi(pControl->wText2));
		szGateway = _strlwr(szGateway);
		
		if(strstr(szLine,szGateway))
		{
		// gateway is correct, do nothing and return true
			delete[] szLine;
			return TRUE;
		}
		else
		{
			// gateway is NOT correct, change gateway to selected gateway if it exists
			// open the gateway select screen
			if(!clickControl(pControl))
				return FALSE;

			int index = 0;
			bool gatefound = false;

			// loop here till we find the right gateway if we can
			pControl = findControl(4, NULL, -1, 257, 500, 292, 160);
			ControlText* cText;
			if(pControl && pControl->pFirstText)
			{
				cText = pControl->pFirstText;
				while(cText)
				{
					char * szGatelist = _strlwr(UnicodeToAnsi(cText->wText));
					if(strstr(szGatelist,szGateway))
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
				// click the correct gateway using the control plus a default x and a y based on (index*24)+12
					if(!clickControl(pControl, -1, 344 + ((index*24)+12)))
						return FALSE;
				}
			}

			// OK Button, gateway select screen	
			if(findControl(6, NULL, -1, 281, 538, 96, 32))
				clickControl(pControl);
			delete[] szLine;
			return TRUE;
		}
	}
	return FALSE;
}

