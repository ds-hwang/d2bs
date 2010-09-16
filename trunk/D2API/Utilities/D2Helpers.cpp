#include <io.h>
#include <errno.h>
#include <ctime>
#include <cmath>
#include <sstream>
#include <algorithm>

#include "D2Helpers.h"
#include "Helpers.h"
#include "D2Intercepts.h"
#include "D2BS.h"
#include "stringhash.h"
#include "D2Ptrs.h"
#include "Control.h"
#include "Console.h"

ClientGameState ClientState(void)
{
	if(*p_D2CLIENT_PlayerUnit && !(*p_D2WIN_FirstControl))
	{
		if(D2CLIENT_GetPlayerUnit()->pInventory &&
				D2CLIENT_GetPlayerUnit()->pPath &&
				D2CLIENT_GetPlayerUnit()->pPath->xPos &&
				D2CLIENT_GetPlayerUnit()->pPath->pRoom1 &&
				D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2 &&
				D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel &&
				D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo)
			return ClientStateInGame;
		else
			return ClientStateBusy;
	}
	else if(!D2CLIENT_GetPlayerUnit() && *p_D2WIN_FirstControl)
		return ClientStateMenu;
	else if(!D2CLIENT_GetPlayerUnit() && !(*p_D2WIN_FirstControl))
	{
		// Throttle since we need to wait anyways.
		Sleep(10);
		return ClientStateBusy;
	}

	Sleep(50);
	return ClientStateNull;
}

bool WaitForClientState(ClientGameState WantedState)
{
	DWORD start = GetTickCount();
	do
	{
		switch(ClientState())
		{
			case ClientStateInGame:
				if(WantedState == ClientStateInGame)
					return true;
			case ClientStateMenu:
				if(WantedState == ClientStateMenu)
					return true;
		}
		Sleep(10);
	} while(Vars.dwGameTimeout == 0 || (Vars.dwGameTimeout > 0 && (GetTickCount() - start) < Vars.dwGameTimeout));

	return false;
}

DWORD GetPlayerArea(void)
{
	return (ClientState() == ClientStateInGame ? D2CLIENT_GetPlayerUnit()->pPath->pRoom1->pRoom2->pLevel->dwLevelNo : NULL);
}

Level* GetLevel(DWORD dwLevelNo)
{
	for(Level* pLevel = D2CLIENT_GetPlayerUnit()->pAct->pMisc->pLevelFirst; pLevel; pLevel = pLevel->pNextLevel)
		if(pLevel->dwLevelNo == dwLevelNo)
			return pLevel;

	return D2COMMON_GetLevel(D2CLIENT_GetPlayerUnit()->pAct->pMisc, dwLevelNo);
}

POINT CalculateTextLen(const char* szwText, int Font)
{
	POINT ret = {0,0};

	if(!szwText)
		return ret;

	wchar_t* Buffer = AnsiToUnicode(szwText);

	DWORD dwWidth, dwFileNo;
	DWORD dwOldSize = D2WIN_SetTextSize(Font);
	ret.y = D2WIN_GetTextSize(Buffer, &dwWidth, &dwFileNo);
	ret.x = dwWidth;
	D2WIN_SetTextSize(dwOldSize);

	delete[] Buffer;
	return ret;
}

int GetSkill(WORD wSkillId)
{
	if(ClientState() != ClientStateInGame)
		return -1;

	for(Skill* pSkill = (*p_D2CLIENT_PlayerUnit)->pSkillInfo->pFirstSkill; pSkill; pSkill = pSkill->pNextSkill)
		if(*pSkill->pSkillId == wSkillId)
			return D2COMMON_GetSkillLevel(*p_D2CLIENT_PlayerUnit, pSkill, true);

	return -1;
}

BOOL SetSkill(WORD wSkillId, BOOL bLeft, DWORD dwItemId = -1)
{
	if(ClientState() != ClientStateInGame) 
		return FALSE;

	if(!GetSkill(wSkillId))
		return FALSE;

	BYTE aPacket[9];

	aPacket[0] = 0x3C;
	*(WORD*)&aPacket[1] = wSkillId;
	aPacket[3] = 0;
	aPacket[4] = (bLeft) ? 0x80 : 0;
	*(DWORD*)&aPacket[5] = dwItemId;

	D2NET_SendPacket(9, 1, aPacket);

	UnitAny* Me = D2CLIENT_GetPlayerUnit();

	int timeout = 0;
	Skill* hand = NULL;
	while(ClientState() == ClientStateInGame && timeout <= 10)
	{
		hand = (bLeft ? Me->pSkillInfo->pLeftSkill : Me->pSkillInfo->pRightSkill);
		if(*hand->pSkillId != wSkillId)
			timeout++;
		else
			return TRUE;

		Sleep(100);
	}
	
	return FALSE;
}

void SendMouseClick(int x, int y, int clicktype)
{
	// HACK: Using PostMessage instead of SendMessage--need to fix this ASAP!
	LPARAM lp = x + (y << 16);
	switch(clicktype)
	{
		case 0: PostMessage(D2GFX_GetHwnd(), WM_LBUTTONDOWN, 0, lp); break;
		case 1: PostMessage(D2GFX_GetHwnd(), WM_LBUTTONUP, 0, lp); break;
		case 2: PostMessage(D2GFX_GetHwnd(), WM_RBUTTONDOWN, 0, lp); break;
		case 3: PostMessage(D2GFX_GetHwnd(), WM_RBUTTONUP, 0, lp); break;
	}
}

POINT GetScreenSize()
{
	POINT result[] = {{800, 600}, {*p_D2CLIENT_ScreenSizeX, *p_D2CLIENT_ScreenSizeY}};
	// the menu is always 800x600
	if(ClientState() == ClientStateMenu)
		return result[0];
	return result[1];
}

int GetScreenSizeX()
{
	return GetScreenSize().x;
}

int GetScreenSizeY()
{
	return GetScreenSize().y;
}

void WorldToScreen(POINT* pPos)
{
	D2COMMON_MapToAbsScreen(&pPos->x, &pPos->y);
	pPos->x -= D2CLIENT_GetMouseXOffset();
	pPos->y -= D2CLIENT_GetMouseYOffset();
}

void ScreenToWorld( POINT *pPos)
{
	D2COMMON_AbsScreenToMap(&pPos->x, &pPos->y);
	pPos->x += D2CLIENT_GetMouseXOffset();
	pPos->y += D2CLIENT_GetMouseYOffset();
}

void ScreenToAutomap(POINT* pPos)
{
	pPos->x = (pPos->x / *p_D2CLIENT_AutomapMode) - p_D2CLIENT_Offset->x + 8;
	pPos->y = (pPos->y / *p_D2CLIENT_AutomapMode) - p_D2CLIENT_Offset->y - 8;
}

void AutomapToScreen(POINT* pPos)
{
	pPos->x = 8 - p_D2CLIENT_Offset->x + (pPos->x * (*p_D2CLIENT_AutomapMode));
	pPos->y = 8 + p_D2CLIENT_Offset->y + (pPos->y * (*p_D2CLIENT_AutomapMode));
}

void D2CLIENT_Interact(UnitAny* pUnit, DWORD dwMoveType)
{
	if(ClientState() != ClientStateInGame) 
		return;

	if(!pUnit)
		return;

	if(!D2CLIENT_FindUnit(pUnit->dwUnitId, pUnit->dwType))
		return;

	InteractInfo pInteract = {
		dwMoveType,
		D2CLIENT_GetPlayerUnit(),
		pUnit,
		D2CLIENT_GetUnitX(pUnit),
		D2CLIENT_GetUnitY(pUnit),
		0, 0
	};

	D2CLIENT_Interact_STUB(&pInteract);
}

BOOL ClickNPCMenu(DWORD NPCClassId, DWORD MenuId)
{
	typedef void (*fnClickEntry) (void);

	if(ClientState() != ClientStateInGame || !D2CLIENT_GetUIState(UI_NPCMENU)) 
		return FALSE;

	NPCMenu* pMenu = (NPCMenu*)p_D2CLIENT_NPCMenu;
	fnClickEntry pClick = (fnClickEntry) NULL;

	for(DWORD i = 0; i < *p_D2CLIENT_NPCMenuAmount; i++)
	{
		if(pMenu->dwNPCClassId == NPCClassId)
		{
			if(pMenu->wEntryId1 == MenuId) pClick = (fnClickEntry)pMenu->dwEntryFunc1;
			else if(pMenu->wEntryId2 == MenuId) pClick = (fnClickEntry)pMenu->dwEntryFunc2;
			else if(pMenu->wEntryId3 == MenuId) pClick = (fnClickEntry)pMenu->dwEntryFunc3;
			else if(pMenu->wEntryId4 == MenuId) pClick = (fnClickEntry)pMenu->dwEntryFunc4;

			if(pClick)
			{
				pClick();
				return TRUE;
			}
		}

		pMenu = (NPCMenu*) ((DWORD)pMenu + sizeof(NPCMenu));
	}

	return FALSE;
}

DWORD GetTileLevelNo(Room2* lpRoom2, DWORD dwTileNo)
{
	if(ClientState() != ClientStateInGame) 
		return 0;

	for(RoomTile* pRoomTile = lpRoom2->pRoomTiles; pRoomTile; pRoomTile = pRoomTile->pNext)
	{
		if(*(pRoomTile->nNum) == dwTileNo)
			return pRoomTile->pRoom2->pLevel->dwLevelNo;
	}

	return 0;
}

UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType)
{
	if(ClientState() != ClientStateInGame) 
		return NULL;

	if(dwId == (DWORD)-1)
		return NULL;

	UnitAny* pUnit = D2CLIENT_FindServerSideUnit(dwId, dwType);
	return pUnit ? pUnit : D2CLIENT_FindClientSideUnit(dwId, dwType);
}

void Print(const char * szFormat, ...)
{
	using namespace std;

	const char REPLACE_CHAR = (unsigned char)0xFE;

	va_list vaArgs;
	va_start(vaArgs, szFormat);
	int len = _vscprintf(szFormat, vaArgs);
	char* str = new char[len+1];
	vsprintf_s(str, len+1, szFormat, vaArgs);
	va_end(vaArgs);

	replace(str, str + len, REPLACE_CHAR, '%');

	// Break into lines through \n.
	list<string> lines;
	string temp;
	stringstream ss(str);

	const int maxlen = 98;
	while(getline(ss, temp))
		SplitLines(temp, maxlen, ' ', lines);

	EnterCriticalSection(&Vars.cPrintSection);

	for(list<string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if(Vars.bUseGamePrint)
		{
			if(ClientState() == ClientStateInGame)
			{
				wchar_t *output = AnsiToUnicode(it->c_str());
				D2CLIENT_PrintGameString(output, 0);
				delete [] output;
			}
			else if(ClientState() == ClientStateMenu && FindControl(4, (char *)NULL, -1, 28, 410, 354, 298))
				D2MULTI_PrintChannelText((char* )it->c_str(), 0);
		}
		else
			Console::AddLine(*it);
	}

	delete[] str;
	str = NULL;

	LeaveCriticalSection(&Vars.cPrintSection);
}

void DrawText(const char* szwText, int x, int y, int color, int font) 
{
	wchar_t* text = AnsiToUnicode(szwText);

	DWORD dwOld = D2WIN_SetTextSize(font);
	D2WIN_DrawText(text, x, y, color, 0);
	D2WIN_SetTextSize(dwOld);

	delete[] text;
}

void DrawAutomapCell(CellFile *cellfile, int xpos, int ypos, BYTE col)
{
	if(!cellfile)return;
	CellContext ct;
	memset(&ct, 0, sizeof(ct));
	ct.pCellFile = cellfile;

	xpos -= (cellfile->cells[0]->width/2);
	ypos += (cellfile->cells[0]->height/2);

	int xpos2 = xpos - cellfile->cells[0]->xoffs, ypos2 = ypos - cellfile->cells[0]->yoffs;
	if ((xpos2 >= GetScreenSizeX()) || ((xpos2 + (int)cellfile->cells[0]->width) <= 0) || (ypos2 >= GetScreenSizeY()) || ((ypos2 + (int)cellfile->cells[0]->height) <= 0)) return;

	static BYTE coltab[2][256];//, tabno = 0, lastcol = 0;
	if (!coltab[0][1]) for (int k = 0; k < 255; k++) coltab[0][k] = coltab[1][k] = (BYTE)k;
	cellfile->mylastcol = coltab[cellfile->mytabno ^= (col != cellfile->mylastcol)][255] = col;

	D2GFX_DrawAutomapCell2(&ct, xpos, ypos, (DWORD)-1, 5, coltab[cellfile->mytabno]);
}

AutomapLayer* InitAutomapLayer(DWORD levelno)
{
	if(ClientState() != ClientStateInGame) 
		return NULL;

	AutomapLayer2 *pLayer = D2COMMON_GetLayer(levelno);
	return D2CLIENT_InitAutomapLayer(pLayer->nLayerNo);
}

double GetDistance(long x1, long y1, long x2, long y2, DistanceType type)
{
	double dist = 0;
	switch(type)
	{
		case Euclidean:
			{
				double dx = (double)(x2 - x1);
				double dy = (double)(y2 - y1);
				dx = pow(dx, 2);
				dy = pow(dy, 2);
				dist = sqrt(dx + dy); 
			}
			break;
		case Chebyshev:
			{
				long dx = (x2 - x1);
				long dy = (y2 - y1);
				dx = abs(dx);
				dy = abs(dy);
				dist = max(dx, dy); 
			}
			break;
		case Manhattan:
			{
				long dx = (x2 - x1);
				long dy = (y2 - y1);
				dx = abs(dx);
				dy = abs(dy);
				dist = (dx + dy);
			}
			break;
		default: 
			dist = -1; 
			break;
	}
 	return dist;
}
