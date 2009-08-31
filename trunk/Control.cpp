#include "Control.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"

#include "debugnew/debug_new.h"

Control* findControl(int Type, char* Text, int Disabled, int PosX, int PosY, int SizeX, int SizeY)
{
	BOOL bFound = FALSE;

	for(Control* pControl = *p_D2WIN_FirstControl; pControl; pControl = pControl->pNext)
	{
		if(Type >= 0 && static_cast<int>(pControl->dwType) == Type)
			bFound = TRUE;
		else if(Type >= 0 && static_cast<int>(pControl->dwType) != Type)
		{
			bFound = FALSE;
			continue;
		}

		if(Text && pControl->dwType == 6)
		{
			if(strcmp(UnicodeToAnsi(pControl->wText2), Text) == 0)
				bFound = TRUE;
			else
			{
				bFound = FALSE;
				continue;
			}
		}

		if(Disabled >= 0 && static_cast<int>(pControl->dwDisabled) == Disabled)
		{
			if(pControl->dwType == 6 && pControl->unkState == 1)
			{
				bFound = FALSE;
				continue;
			}
			bFound = TRUE;
		}
		else if(Disabled >= 0 && static_cast<int>(pControl->dwDisabled) != Disabled)
		{			
			bFound = FALSE;
			continue;
		}

		if(PosX >= 0 && static_cast<int>(pControl->dwPosX) == PosX)
			bFound = TRUE;
		else if(PosX >= 0 && static_cast<int>(pControl->dwPosX) != PosX)
		{
			bFound = FALSE;
			continue;
		}

		if(PosY >= 0 && static_cast<int>(pControl->dwPosY) == PosY)
			bFound = TRUE;
		else if(PosY >= 0 && static_cast<int>(pControl->dwPosY) != PosY)
		{
			bFound = FALSE;
			continue;
		}

		if(SizeX >= 0 && static_cast<int>(pControl->dwSizeX) == SizeX)
			bFound = TRUE;
		else if(SizeX >= 0 && static_cast<int>(pControl->dwSizeX) != SizeX)
		{
			bFound = FALSE;
			continue;
		}

		if(SizeY >= 0 && static_cast<int>(pControl->dwSizeY) == SizeY)
			bFound = TRUE;
		else if(SizeY >= 0 && static_cast<int>(pControl->dwSizeY) != SizeY)
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
	//delay for fist char box and char info fillin
	// should prevent trying to select char on realm down. bobite
	bool found= false;
	int timeout = 0;
	Control* pControl = findControl(4, NULL, -1, 37, 178, 200, 92);
	while(!found){
		Sleep(500);
		pControl = findControl(4, NULL, -1, 37, 178, 200, 92);
		if(pControl->pFirstText != NULL && pControl->pFirstText->pNext != NULL){
			found= true;
		}
		timeout++;
		if (timeout > 30)
			return false;
	}
	// Select the first control on the character selection screen.
	 pControl = findControl(4, NULL, -1, 37, 178, 200, 92);
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
			if(pControl = findControl(6, NULL, -1, 281, 538, 96, 32))
			{
				if(!clickControl(pControl))
				{
					delete[] szLine;
					return FALSE;
				}
			}
			else
			{
				delete[] szLine;
				return FALSE;
			}
			delete[] szLine;
			return TRUE;
		}
	}
	return FALSE;
}

