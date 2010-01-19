#ifndef D2HELPERS_H
#define D2HELPERS_H

#include "D2Structs.h"

enum DistanceType
{
	Euclidean, Chebyshev, Manhattan
};

enum ClientGameState
{
	ClientStateNull, ClientStateMenu, ClientStateInGame, ClientStateBusy
};

VOID	Log(CHAR* szFormat, ...);

WORD	GetUnitX(UnitAny* pUnit);
WORD	GetUnitY(UnitAny* pUnit);
ClientGameState ClientState(VOID);
BOOL	GameReady(VOID);
DWORD	GetPlayerArea(VOID);
VOID	ScreenSizeCheck(POINT* pPoint);
VOID	SendMouseClick(INT x, INT y, INT clicktype);
POINT	CalculateTextLen(CHAR* szwText, INT Font);
VOID	MapToScreenCoords(POINT* pPos);
VOID	ScreenToAutomap(POINT *ptPos, int x, int y);
Level*	GetLevel(DWORD dwLevelNo);
BOOL	IsTownLevel(INT nLevel);
VOID	D2CLIENT_Interact(UnitAny* pUnit, DWORD dwMoveType);
VOID	myDrawText(char* szwText, int x, int y, int color, int font);
VOID	myDrawCenterText(char* szwText, int x, int y, int color, int font, int div) ;
UnitAny* FindItemByPosition(DWORD x, DWORD y, DWORD Location);
BYTE	CalcPercent(DWORD dwVal, DWORD dwMaxVal, BYTE iMin = NULL);
DWORD	GetTileLevelNo(Room2* lpRoom2, DWORD dwTileNo);

BOOL	ClickNPCMenu(DWORD NPCClassId, DWORD MenuId);
INT		GetItemLocation(UnitAny *pItem);
VOID	SelectInventoryItem(DWORD x, DWORD y, DWORD dwLocation);

INT		GetSkill(WORD wSkillId);
BOOL	SetSkill(WORD wSkillId, BOOL bLeft);
CHAR*	GetSkillByID(WORD id);
WORD	GetSkillByName(CHAR* szSkillName);

const char* GetUnitName(UnitAny* pUnit, CHAR* szBuf, size_t bufSize);
void GetItemCode(UnitAny* pUnit, char* szBuf);

InventoryLayout* GetLayoutFromTable(DWORD dwTable);
UnitAny* D2CLIENT_FindUnit(DWORD dwId, DWORD dwType);

POINT GetScreenSize();
INT D2GetScreenSizeX();
INT D2GetScreenSizeY();

CellFile* LoadCellFile(CHAR* lpszPath, DWORD bMPQ = 3);
int RemoveColorSpecs(wchar_t *lpwsz);

AutomapLayer* InitAutomapLayer(DWORD levelno);
DWORD __fastcall D2CLIENT_InitAutomapLayer_STUB(DWORD nLayerNo);
VOID myDrawAutomapCell(CellFile *cellfile, int xpos, int ypos, BYTE col);
DWORD ReadFile(HANDLE hFile, void *buf, DWORD len);
VOID *memcpy2(void *dest, const void *src, size_t count);
HANDLE OpenFileRead(char *filename);
BYTE *AllocReadFile(char *filename);
CellFile *LoadBmpCellFile(BYTE *buf1, int width, int height);
CellFile *LoadBmpCellFile(char *filename);
CellFile *myInitCellFile(CellFile *cf);

DWORD __fastcall D2CLIENT_GetUnitName_STUB(DWORD UnitAny);
DWORD __fastcall D2CLIENT_GetUIVar_STUB(DWORD varno);
DWORD __fastcall D2CLIENT_LoadUIImage_ASM(char* Path);
VOID  __fastcall D2CLIENT_SetSelectedUnit_STUB(DWORD UnitAny);
VOID  __fastcall D2CLIENT_Interact_ASM(DWORD Struct);
DWORD __fastcall D2CLIENT_clickParty_ASM(DWORD RosterUnit, DWORD Mode);
DWORD __fastcall FindUnit_STUB(DWORD unitid, DWORD unittype);
VOID  __fastcall D2CLIENT_ClickShopItem_ASM(DWORD x, DWORD y, DWORD BuyOrSell);
VOID  __fastcall D2CLIENT_ShopAction_ASM(DWORD pTable, DWORD pItem, DWORD pNPC, DWORD _1, DWORD pTable2 /* Could be also the ItemCost?*/, DWORD dwMode, DWORD _2, DWORD _3);
VOID  __fastcall D2CLIENT_clickItemRight_ASM(DWORD x, DWORD y, DWORD Location, DWORD pItem, DWORD pItemPath);
VOID  __fastcall D2CLIENT_clickBelt(DWORD x, DWORD y, DWORD IventoryData);
VOID  __fastcall D2CLIENT_clickBeltRight_ASM(DWORD pInventory, DWORD pPlayer, DWORD HoldShift, DWORD dwPotPos);
VOID  __fastcall D2CLIENT_GetItemDesc_ASM(DWORD pUnit, wchar_t* pBuffer);
VOID  __fastcall D2COMMON_DisplayOverheadMsg_ASM(DWORD pUnit);
VOID  __fastcall D2CLIENT_MercItemAction_ASM(DWORD bPacketType, DWORD dwSlot);
DWORD __fastcall D2CLIENT_LoadUIImage_ASM(CHAR* lpszPath);
VOID __fastcall D2CLIENT_PlaySound(DWORD dwSoundId);
VOID __fastcall D2CLIENT_TakeWP(DWORD dwUnitId, DWORD dwLevelId);
DWORD __fastcall TestPvpFlag_STUB(DWORD planum1, DWORD planum2, DWORD flagmask);
VOID __fastcall DrawRectFrame_STUB(RECT* rect);
VOID __stdcall myClickMap_ASM(DWORD MouseFlag, DWORD x, DWORD y, DWORD Type);
DWORD D2CLIENT_GetMinionCount(UnitAny* pUnit, DWORD dwType);
VOID __fastcall D2CLIENT_HostilePartyUnit(RosterUnit* pUnit, DWORD dwButton);
VOID __stdcall D2CLIENT_TakeWaypoint(DWORD dwWaypointId, DWORD dwArea);

double GetDistance(long x1, long y1, long x2, long y2, DistanceType type = Euclidean);

#endif
