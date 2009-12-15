#pragma once

#include "D2Structs.h"

Control* findControl(int Type, int LocaleID, int Disabled, int PosX, int PosY, int SizeX, int SizeY);
Control* findControl(int Type, char* Text, int Disabled, int PosX, int PosY, int SizeX, int SizeY);
bool clickControl(Control* pControl, int x = -1, int y = -1);
BOOL OOG_SelectCharacter(char * szCharacter);
BOOL OOG_SelectGateway(char * szGateway, size_t strSize);
void SetControlText(Control* pControl, char* Text);
int OOG_GetLocation(void);
bool OOG_CreateGame(const char* name, const char* pass, int difficulty);
bool OOG_JoinGame(const char* name, const char* pass);
