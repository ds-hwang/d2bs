#pragma once

Control* findControl(DWORD dwType, DWORD dwX, DWORD dwY, DWORD dwSizeX, DWORD dwSizeY);
bool clickControl(Control* pControl, int x = -1, int y = -1);
BOOL OOG_SelectCharacter(char * szCharacter);
void SetControlText(Control* control, char* text);
