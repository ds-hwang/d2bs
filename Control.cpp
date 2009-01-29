#include "D2BS.h"

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

void clickControl( Control * pControl )
{
	SendMouseClick(pControl->dwPosX + (pControl->dwSizeX / 2), pControl->dwPosY - (pControl->dwSizeY / 2), 0);
	SendMouseClick(pControl->dwPosX + (pControl->dwSizeX / 2), pControl->dwPosY - (pControl->dwSizeY / 2), 1);
}