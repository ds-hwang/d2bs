#pragma once

#include "D2Structs.h"

Control* findControl(int Type, char* Text, int Disabled, int PosX, int PosY, int SizeX, int SizeY);
bool clickControl(Control* pControl, int x = -1, int y = -1);
BOOL OOG_SelectCharacter(char * szCharacter);
BOOL OOG_SelectGateway(char * szGateway);
void SetControlText(Control* pControl, char* Text);

