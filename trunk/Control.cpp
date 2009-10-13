#include "Control.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "Helpers.h"
#include "Constants.h"
#include "debugnew/debug_new.h"

Control* findControl(int Type, int LocaleID, int Disabled, int PosX, int PosY, int SizeX, int SizeY)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	char* localeStr = NULL; 
	if(LocaleID >= 0)
	{
		localeStr = UnicodeToAnsi(D2LANG_GetLocaleText((WORD)LocaleID));
		if(!localeStr)
			return NULL;
		Control* res = findControl(Type, localeStr, Disabled, PosX, PosY, SizeX, SizeY);
		delete[] localeStr;
		return res;
	}

	return NULL;
}

Control* findControl(int Type, char* Text, int Disabled, int PosX, int PosY, int SizeX, int SizeY)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

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

		if(Text && pControl->dwType == 6)
		{
			char* text2 = UnicodeToAnsi(pControl->wText2);
			if(!text2)
				return NULL;
			if(strcmp(text2, Text) == 0)
			{
				bFound = TRUE;
				delete[] text2;
			}
			else
			{
				bFound = FALSE;
				delete[] text2;
				continue;
			}
		}

		if(Text && pControl->dwType == 4)
		{
			if(pControl->pFirstText != NULL && pControl->pFirstText->wText != NULL)
			{
				char* text2 = UnicodeToAnsi(pControl->pFirstText->wText);
				if(!text2)
					return NULL;
				if(strstr(Text, text2) != 0)
				{
					bFound = TRUE;
					delete[] text2;
				}
				else
				{
					bFound = FALSE;
					delete[] text2;
					continue;
				}
			}
			else
			{
				bFound = FALSE;
				continue;
			}
		}		
		if(bFound)
			return pControl;
	}

	return NULL;
}

bool clickControl(Control* pControl, int x, int y)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	if(pControl)
	{
		if(x == -1)
			x = pControl->dwPosX + (pControl->dwSizeX / 2);
		if(y == -1)
			y = pControl->dwPosY - (pControl->dwSizeY / 2);
		
		Sleep(100);
		SendMouseClick(x, y, 0);
		Sleep(100);
		SendMouseClick(x, y, 1);
		Sleep(100);
		
		return true;
	}
	return false;
}

BOOL OOG_SelectCharacter(char * szCharacter)
{
	if(ClientState() != ClientStateMenu)
		return NULL;

	// Select the first control on the character selection screen.
	Control* pControl = findControl(4, (char *)NULL, -1, 237, 178, 72, 93);
	ControlText* cText;

	while (pControl != NULL)
	{
		if(pControl->dwType == 4 && pControl->pFirstText != NULL && pControl->pFirstText->pNext != NULL)
			cText = pControl->pFirstText->pNext;
		else
			cText = NULL;

		if(cText != NULL)
		{
			char * szLine = UnicodeToAnsi(cText->wText);
			if(!szLine)
				return FALSE;
			if(strlen(szLine) == strlen(szCharacter) && strstr(szLine,szCharacter) != NULL)
			{
				delete[] szLine;
				if(!clickControl(pControl))
					return FALSE;

				// OK Button
				pControl = findControl(6, (char *)NULL, -1, 627, 572, 128, 35);
				if(pControl)
				{
					if(!clickControl(pControl))
						return FALSE;

					return TRUE;
				}
				else
					return FALSE;

			}
			else
				delete[] szLine;
		}
		pControl = pControl->pNext;
	}
	return FALSE;
}

void SetControlText(Control* pControl, char* Text)
{
	if(ClientState() != ClientStateMenu)
		return;

	if(pControl && Text)
	{
		wchar_t* szwText = AnsiToUnicode(Text);
		if(!szwText)
			return;
		D2WIN_SetControlText(pControl, szwText);
		delete[] szwText;
	}
}

BOOL OOG_SelectGateway(char * szGateway, size_t strSize)
{
	if(ClientState() != ClientStateMenu)
		return FALSE;

	if(strstr(szGateway,"ERROR"))
		return FALSE;
	// Select the gateway control.
	Control* pControl = findControl(6, (char *)NULL, -1, 264, 391, 272, 25);

	// if the control exists and has the text label, check if it matches the selected gateway
	if(pControl && pControl->wText2)
	{
		char* szLine = UnicodeToAnsi(pControl->wText2);
		if(!szLine)
			return FALSE;

		szLine = _strlwr(szLine);
		_strlwr_s(szGateway, strSize);
		if(strstr(szLine, szGateway))
		{
			// gateway is correct, do nothing and return true
			delete[] szLine;
			return TRUE;
		}
		else
		{
			delete[] szLine;
			// gateway is NOT correct, change gateway to selected gateway if it exists
			// open the gateway select screen
			if(!clickControl(pControl))
				return FALSE;

			int index = 0;
			bool gatefound = false;

			// loop here till we find the right gateway if we can
			pControl = findControl(4, (char *)NULL, -1, 257, 500, 292, 160);
			ControlText* cText;
			if(pControl && pControl->pFirstText)
			{
				cText = pControl->pFirstText;
				while(cText)
				{
					char * szGatelist = UnicodeToAnsi(cText->wText);
					if(!szGatelist)
						return FALSE;
					szGatelist = _strlwr(szGatelist);
					if(strstr(szGatelist, szGateway))
					{
						// chosen gateway IS in the list and matches, cleanup and break the loop
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
					if(!clickControl(pControl, -1, 344+((index*24)+12)))
						return FALSE;
				}
			}

			// OK Button, gateway select screen
			pControl = findControl(6, (char *)NULL, -1, 281, 538, 96, 32);
			if(pControl)
			{
				if(!clickControl(pControl))
					return FALSE;
			}
			else
				return FALSE;

			return TRUE;
		}
	}
	return FALSE;
}
int OOG_GetLocation()
{
	if(ClientState() != ClientStateMenu)
		return OOG_NONE;

	if(findControl(6, 5103, -1, 330, 416, 128, 35))
		return OOG_MAIN_MENU_CONNECTING;					//21 Connecting to Battle.net	
	else if(findControl(6, 5102, -1, 335, 412, 128, 35))
		return OOG_LOGIN_ERROR;								//10 Login Error	
	else if(findControl(6, 5103, -1, 433, 433, 96, 32))
	{ 
		if (findControl(4, (char *)NULL, -1, 427,234,300,100))
			return OOG_INLINE;								//2 waiting in line	
		else if(findControl(4, 10018, -1, 459, 380, 150, 12))
			return OOG_CREATE;								//4 Create game
		else if(findControl(6, 5119, -1, 594, 433, 172, 32))
			return OOG_JOIN;								// 5 Join Game
		else if(findControl(6, 5102, -1, 671, 433, 96, 32))
			return OOG_CHANNEL;								//7 "Channel List"
		else
			return OOG_LADDER;								//6 "Ladder"		
	}	
	else if(findControl(6, 5103, -1, 351,337,96,32))		//5103 = CANCEL
	{
		if(findControl(4, 5243, -1, 268, 300, 264, 100))
			return OOG_CHARACTER_SELECT_PLEASE_WAIT;		//16 char select please wait...
		if (findControl(4, (char *)NULL, -1, 268,320,264,120))
			return OOG_PLEASE_WAIT;							//25 "Please Wait..."single player already exists also
	}
		else if (findControl(6, 5102, -1, 351,337,96,32))	//5102 =OK
	{
		if (findControl(4, 5131, -1, 268,320,264,120))	
			return OOG_LOST_CONNECTION;						//17 lost connection	
		else if (findControl(4, 5347, -1, 268,320,264,120))
			return OOG_DISCONNECTED;						//14  Disconnected
		else
			return OOG_CHARACTER_CREATE_ALREADY_EXISTS;		//30 Character Create - Dupe Name									
	}
	else if(findControl(6, 5101, -1, 33,572,128,35))		//5101 = EXIT
	{
		if(findControl(6, 5288, -1, 264, 484, 272, 35))
			return OOG_LOGIN;								//9 Login
		if (findControl(6, 5102, -1, 495,438,96,32))
			return OOG_CHARACTER_SELECT_CHANGE_REALM;		//43 char select change realm						
		if(findControl(6, 5102, -1, 627,572,128,35) && findControl(6, 10832, -1, 33,528,168,60)) //10832=create new
		{
			if (findControl(6, 10018, -1, 264,297,272,35)) //NORMAL
				return OOG_DIFFICULTY;						//20 single char Difficulty
			Control* pControl = findControl(4, (char *)NULL, -1, 37, 178, 200, 92);
			if(pControl && pControl->pFirstText && pControl->pFirstText->pNext)
				return OOG_CHAR_SELECT;						//12 char select
			else
			{
				if (findControl(4, 11162, -1,45,318,531,140) || findControl(4, 11066, -1,45,318,531,140))	
					return OOG_REALM_DOWN;
				else				
					return OOG_CHARACTER_SELECT_NO_CHARS;	//42 char info not loaded 
			}
		}
		if(findControl(6, 5101, -1, 33, 572, 128, 35))		//5101=Exit
		{
			if (findControl(6, 5102, 0, 627,572,128,35))			//5102=ok
				return OOG_GATEWAY;							//27 char create screen with char selected
			else
			{
				if(findControl(4, 5226, -1, 321, 448, 300, 32))
					return OOG_NEW_ACCOUNT;					//32 create new bnet account
				else
					return OOG_NEW_CHARACTER;				//15 char create screen no char selected
			}
		}
	}
	if(findControl(6, 5102, -1, 335, 450, 128, 35))
	{
		if(findControl(4, 5200, -1, 162, 270, 477, 50))
			return OOG_CDKEY_IN_USE;						//19 CD-KEY in use
		else if (findControl(4, 5190, -1, 162,420,477,100))		 //5190="If using a modem"
			return OOG_UNABLE_TO_CONNECT;					//11 unable to connect
		else		
			return OOG_INVALID_CDKEY;						//22 invalid CD-KEY 		
	}	
	else if (findControl(4, 5159, -1, 438, 300, 326, 150)) 
		return OOG_GAME_DOES_NOT_EXIST;						//28 game dosent exist
	else if (findControl(4, 5161, -1, 438, 300, 326, 150)) 
		return OOG_GAME_IS_FULL;							//38 Game is full
	else if (findControl(4, 5138, -1, 438, 300, 326, 150)) 
		return OOG_GAME_EXIST;								//26 Game already exists
	else if (findControl(4, 5139, -1, 438, 300, 326, 150)) 
		return OOG_SERVER_DOWN;								//24 server down	
	else if (findControl(6, 5106, -1, 264,324,272,35))				//5106="SINGLE PLAYER"
		return OOG_MAIN_MENU;								//8 Main Menu
	else if (findControl(6, 11126, -1, 27,480,120,20))					//11126=ENTER CHAT
		return OOG_LOBBY;									//1 base bnet 
	else if (findControl(6, 5308, -1, 187,470,80,20))					//5308="HELP"
		return OOG_CHAT;									//3 chat bnet 
	else if(findControl(4, 21882, -1, 100, 580, 600, 80))
		return OOG_D2SPLASH;								//18 Spash	
	else if (findControl(6, 5102, -1, 281,538,96,32))
		return OOG_GATEWAY;									//27 select gateway
	else if (findControl(6, 5181, -1, 525,513,128,35))
		return OOG_AGREE_TO_TERMS;							//31 agree to terms
	else if (findControl(6, 5102, -1, 525,513,128,35))
		return OOG_PLEASE_READ;								//33 please read
	else if (findControl(6, 11097, -1, 265,527,272,35))
		return OOG_REGISTER_EMAIL;							//34 register email
	else if (findControl(6, 5101, -1, 33,578,128,35))
		return OOG_CREDITS;									//35 Credits
	else if (findControl(6, 5103, -1, 334,488,128,35))
		return OOG_CINEMATICS;								//36 Cinematics
	else if (findControl(6, 5116, -1, 264,350,272,35))
		return	OOG_OTHER_MULTIPLAYER;						//39 other multi player
	else if (findControl(6, 5103, -1,281,337,96,32))
		return OOG_ENTER_IP_ADDRESS;						//41 enter ip	
	else if (findControl(6, 5118, -1,265,206,272,35))
		return	OOG_TCP_IP;									//40 tcp-ip
	
	return OOG_NONE;
}
