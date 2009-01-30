#pragma once

Control* findControl(DWORD dwType, DWORD dwX, DWORD dwY, DWORD dwSizeX, DWORD dwSizeY);
bool clickControl(Control* pControl);
BOOL OOG_SelectCharacter(char * szCharacter);
void SetControlText(Control* control, char* text);
