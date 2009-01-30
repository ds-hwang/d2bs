#pragma once

extern Control* findControl(DWORD dwType, DWORD dwX, DWORD dwY, DWORD dwSizeX, DWORD dwSizeY);
extern void clickControl( Control * pControl );
extern BOOL OOG_SelectCharacter(char * szCharacter);