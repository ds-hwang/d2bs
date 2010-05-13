#ifndef D2HELPERS_H
#define D2HELPERS_H

#include "D2Structs.h"
#include "Stubs.h"
#include "CellFile.h"
#include "Constants.h"

enum DistanceType
{
	Euclidean, Chebyshev, Manhattan
};

enum ClientGameState
{
	ClientStateNull, ClientStateMenu, ClientStateInGame, ClientStateBusy
};

ClientGameState ClientState(void);
bool WaitForClientState(ClientGameState WantedState = ClientStateInGame);

DWORD	GetPlayerArea(void);
void	SendMouseClick(int x, int y, int clicktype);
POINT	CalculateTextLen(const char* szwText, int Font);

void	WorldToScreen(POINT* pPos);
void	ScreenToWorld(POINT *ptPos);

void	ScreenToAutomap(POINT* pPos);
void	AutomapToScreen(POINT* pPos);

void	D2CLIENT_Interact(UnitAny* pUnit, DWORD dwMoveType);
Level*	GetLevel(DWORD dwLevelNo);
DWORD	GetTileLevelNo(Room2* lpRoom2, DWORD dwTileNo);

BOOL	ClickNPCMenu(DWORD NPCClassId, DWORD MenuId);

inline int GetUnitHP(UnitAny* pUnit) { return (int)(D2COMMON_GetUnitStat(pUnit, STAT_HP, 0) >> 8); }
inline int GetUnitMP(UnitAny* pUnit) { return (int)(D2COMMON_GetUnitStat(pUnit, STAT_MANA, 0) >> 8); }

int		GetSkill(WORD wSkillId);
BOOL	SetSkill(WORD wSkillId, BOOL bLeft);

UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType);

POINT GetScreenSize();
int GetScreenSizeX();
int GetScreenSizeY();

void	DrawText(const char* szwText, int x, int y, int color, int font);
void	DrawAutomapCell(CellFile *cellfile, int xpos, int ypos, BYTE col);

AutomapLayer* InitAutomapLayer(DWORD levelno);
double GetDistance(long x1, long y1, long x2, long y2, DistanceType type = Euclidean);

#endif
