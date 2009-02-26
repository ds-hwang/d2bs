// this file is for function pointers that are directly called by our code

#pragma once

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "Macros.h"
#include "Structs.h"
#include "Pointers.h"

// wrapper functions to handle odd calling convention functions
DWORD __fastcall TestPvpFlag(DWORD planum1, DWORD planum2, DWORD flagmask);
DWORD __fastcall GetUIVar(DWORD varno);
#define GetUIState GetUIVar

// actual functions
FUNCTION(D2Client, 0x1390,  MonsterTxt*, __fastcall, GetMonsterTxt, (DWORD monno))
FUNCTION(D2Client, 0x71740, void, __stdcall, PrintGameString, (wchar_t *wMessage, int nColor))
FUNCTION(D2Client, 0x71500, void, __stdcall, PrintPartyString, (wchar_t *wMessage, int nColor))
FUNCTION(D2Client, 0x73EE0, void, __stdcall, PrintGameString2, (char *szMessage))
FUNCTION(D2Client, 0x5D650, int, __stdcall, GetPlayerXOffset, (void))
FUNCTION(D2Client, 0x5D670, int, __stdcall, GetPlayerYOffset, (void))
FUNCTION(D2Client, 0x6EA80, void, __fastcall, SetSelectedUnit_I, (UnitAny *pUnit))
FUNCTION(D2Client, 0x6ECA0, UnitAny*, __stdcall, GetSelectedUnit, (void))
FUNCTION(D2Client, 0x79CD0, BYTE, __stdcall, GetDifficulty, (void))
FUNCTION(D2Client, 0x30080, DWORD, __fastcall, GetUnknownFlag, (void))
FUNCTION(D2Client, 0x120F0, GameStructInfo*, __stdcall, GetGameInfo, (void))
FUNCTION(D2Client, 0x3CFC0, DWORD, __stdcall, GetAutomapSize, (void))
FUNCTION(D2Client, 0x3D5B0, AutomapCell*, __fastcall, NewAutomapCell, (void))
FUNCTION(D2Client, 0x3F090, void, __fastcall, AddAutomapCell, (AutomapCell *aCell, AutomapCell **node))
FUNCTION(D2Client, 0x404C0, void, __stdcall, RevealAutomapRoom, (Room1 *pRoom1, DWORD dwClipFlag, AutomapLayer *aLayer))
FUNCTION(D2Client, 0x40650, AutomapLayer*, __fastcall, InitAutomapLayer_I, (DWORD nLayerNo))
FUNCTION(D2Client, 0xC3160, DWORD, __fastcall, GetMonsterOwner, (DWORD nMonsterId))
FUNCTION(D2Client, 0x8AA90, DWORD, __fastcall, GetUiVar_I, (DWORD dwVarNo))
FUNCTION(D2Client, 0x8EF00, DWORD, __fastcall, SetUIState, (DWORD varno, DWORD howset, DWORD unknown1))
FUNCTION(D2Client, 0xAB3C0, void, __stdcall, GetItemNameString, (UnitAny *pItem, wchar_t *wItemName, int nLen))
FUNCTION(D2Client, 0xBB4F0, void, __stdcall, CalculateShake, (DWORD *dwPosX, DWORD *dwPosY))
FUNCTION(D2Client, 0x1E490, UnitAny*, __stdcall, GetPlayerUnit, (void))
FUNCTION(D2Client, 0x40650, AutomapLayer*, __fastcall, InitLevel_I, (void))
FUNCTION(D2Client, 0x8AB50, void, __fastcall, DrawRectFrame, (DWORD Rect))
FUNCTION(D2Client, 0x7AB00, void, __fastcall, ExitGame, (void))
FUNCTION(D2Client, 0x5B050, void, __stdcall, Attack, (AttackStruct* Attack, BOOL AttackingUnit))
FUNCTION(D2Client, 0xAB3C0, BOOL, __stdcall, GetItemName, (UnitAny* pItem, wchar_t* wBuffer, DWORD dwSize))
FUNCTION(D2Client, 0xC2610, void, __fastcall, AcceptTrade, (void))
FUNCTION(D2Client, 0x56440, void, __fastcall, CancelTrade, (void))
FUNCTION(D2Client, 0x5D670, DWORD, __fastcall, GetMouseXOffset, (void))
FUNCTION(D2Client, 0x5D680, DWORD, __fastcall, GetMouseYOffset, (void))
FUNCTION(D2Client, 0x1B4C0, void, __fastcall, ClearScreen, (void))
FUNCTION(D2Client, 0x17D00, void*, __stdcall, GetQuestInfo, (void))
FUNCTION(D2Client, 0x20010, DWORD, __stdcall, UnitTestSelect, (UnitAny* pUnit, DWORD _1, DWORD _2, DWORD _3))
FUNCTION(D2Client, 0x1F1C0, UnitAny*, __fastcall, FindServerSideUnit, (DWORD dwId, DWORD dwType))
FUNCTION(D2Client, 0x1F1A0, UnitAny*, __fastcall, FindClientSideUnit, (DWORD dwId, DWORD dwType))
FUNCTION(D2Client, 0x8EF00, DWORD, __fastcall, SetUIVar, (DWORD varno, DWORD howset, DWORD unknown1))
FUNCTION(D2Client, 0x5CE80, void, __stdcall, ClickMap, (DWORD MouseFlag, DWORD x, DWORD y, DWORD Type))
FUNCTION(D2Client, 0x9EB40, UnitAny*, __fastcall, GetCursorItem, (void))
FUNCTION(D2Client, 0xB0900, void, __stdcall, LeftClickItem, (UnitAny* pPlayer, Inventory* pInventory, INT x, INT y, DWORD dwClickType, InventoryLayout* pLayout, DWORD Location))
FUNCTION(D2Client, 0x1A630, void, __fastcall, CloseNPCInteract, (void))
FUNCTION(D2Client, 0x7BBD0, void, __fastcall, CloseInteract, (void))
FUNCTION(D2Client, 0x86F80, DWORD, __stdcall, ChatBoxHandler, (MSG* pMsg))
FUNCTION(D2Client, 0xA8B70, void, __fastcall, InitInventory, (void))
FUNCTION(D2Client, 0x19D90, void, __fastcall, FixShopActions, (void))
FUNCTION(D2Client, 0x18410, void, __fastcall, SubmitItem, (DWORD dwItemId))
FUNCTION(D2Client, 0xC3040, DWORD, __fastcall, GetUnitHPPercent, (DWORD dwUnitId))
FUNCTION(D2Client, 0xB1B30, UnitAny*, __fastcall, GetMercUnit, (void))
FUNCTION(D2Client, 0x19E00, void, __fastcall, ShopAction, (UnitAny* pItem, UnitAny* pNpc, UnitAny* pNpc2, DWORD dwSell, DWORD dwItemCost, DWORD dwMode, DWORD _2, DWORD _3))
FUNCTION(D2Client, 0x18450, UnitAny*, __fastcall, GetCurrentInteractingNPC, (void))
FUNCTION(D2Client, 0x8C1F0, void, __fastcall, PerformGoldDialogAction, (void))
FUNCTION(D2Client, 0x79160, void, __fastcall, LeaveParty, (void))
FUNCTION(D2Client, 0xA6560, void, __fastcall, Transmute, (void))

FUNCTION(D2Common, -10184,  void, __stdcall, AddRoomData, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom))
FUNCTION(D2Common, -11009,  void, __stdcall, RemoveRoomData, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom))
FUNCTION(D2Common, 0x73A10, LevelTxt*, __stdcall, GetLevelTxt, (DWORD levelno))
FUNCTION(D2Common, 0x37300, ObjectTxt*, __stdcall, GetObjectTxt, (DWORD objno))
FUNCTION(D2Common, 0x73870, AutomapLayer2*, __fastcall, GetLayer, (DWORD dwLevelNo))
FUNCTION(D2Common, -11020,  Level*, __fastcall, GetLevel, (ActMisc *pMisc, DWORD dwLevelNo))
FUNCTION(D2Common, -10721,  void, __stdcall, InitLevel, (Level *pLevel))
FUNCTION(D2Common, -11151,  UnitAny*, __stdcall, GetItemFromInventory, (Inventory* inv))
FUNCTION(D2Common, -10770,  UnitAny*, __stdcall, GetNextItemFromInventory, (UnitAny *pItem))
FUNCTION(D2Common, 0x5C980, ItemTxt*, __stdcall, GetItemText, (DWORD dwItemNo))
FUNCTION(D2Common, 0x34E20, DWORD, __stdcall, GetUnitStat, (UnitAny* pUnit, DWORD dwStat, DWORD dwStat2))
FUNCTION(D2Common, -10178,  int, __stdcall, GetUnitX, (Path *hPath))
FUNCTION(D2Common, -10191,  int, __stdcall, GetUnitY, (Path *hPath))
FUNCTION(D2Common, -10912,  void, __stdcall, MapToAbsScreen, (long *pX, long *pY))
FUNCTION(D2Common, -10725,  void, __stdcall, AbsScreenToMap, (long *ptMouseX, long *ptMouseY))
FUNCTION(D2Common, 0x56550, BOOL, __stdcall, AddLevel, (Level* pLevel))
FUNCTION(D2Common, 0x56780, Act*, __stdcall, LoadAct, (DWORD ActNumber, DWORD MapId, DWORD Unk, DWORD Unk_2, DWORD Unk_3, DWORD Unk_4, DWORD TownLevelId, DWORD Func_1, DWORD Func_2))
FUNCTION(D2Common, -10366,  Room1*, __stdcall, GetRoomFromUnit, (UnitAny * ptUnit))
FUNCTION(D2Common, -10904,  int, __stdcall, GetSkillLevel, (UnitAny* pUnit, Skill* pSkill, BOOL bTotal))
FUNCTION(D2Common, -10233,  DWORD, __stdcall, CheckUnitCollision, (UnitAny* pUnitA, UnitAny* pUnitB, DWORD dwBitMask))
FUNCTION(D2Common, -10494,  DWORD, __stdcall, GetBaseStat, (UnitAny *Unit, DWORD dwStat, DWORD dwUkn))
FUNCTION(D2Common, -10310,  UnitAny*, __stdcall, GetCursorItem, (Inventory * ptInventory))
FUNCTION(D2Common, -10295,  int, __stdcall, GetUnitState, (UnitAny *pUnit, DWORD dwStateNo))
FUNCTION(D2Common, -10554,  DWORD, __stdcall, GetItemFlag, (UnitAny *Item, DWORD dwFlagMask, DWORD dwLineNo, char *szFileName))
FUNCTION(D2Common, -10852,  UnitAny*, __stdcall, GetItemUnit, (DWORD dwId))
FUNCTION(D2Common, -10611,  StatList*, __stdcall, GetStatList, (UnitAny* pUnit, DWORD dwUnk, DWORD dwMaxEntries))
FUNCTION(D2Common, -10373,  DWORD, __stdcall, CopyStatList, (StatList* pStatList, Stat* pStatArray, DWORD dwMaxEntries))
FUNCTION(D2Common, -10276,  DWORD, __stdcall, GetItemQuality, (UnitAny* pItem))
FUNCTION(D2Common, 0x2C440, void, __stdcall, GetItemNameString, (UnitAny *pItem, char *szItemName, int nLen))
FUNCTION(D2Common, -11065,  char*, __stdcall, GetItemMagicalMods, (WORD wPrefixNum))
FUNCTION(D2Common, -10875,  DWORD, __stdcall, CheckWaypoint, (DWORD WaypointTable, DWORD dwLevelId))
FUNCTION(D2Common, -10122,  DWORD, __stdcall, GetItemPrice, (UnitAny* MyUnit, UnitAny* pItem, DWORD U1_,DWORD U2_,DWORD U3_,DWORD U4_))
FUNCTION(D2Common, -10600,  int, __stdcall, GetQuestFlag, (void* QuestInfo, DWORD dwAct, DWORD dwQuest))
FUNCTION(D2Common, -10041,  OverheadMsg*, __stdcall, GenerateOverheadMsg, (DWORD dwUnk, CHAR* szMsg, DWORD dwTrigger))
FUNCTION(D2Common, -10315,  void, __stdcall, FixOverheadMsg, (OverheadMsg* pMsg, DWORD dwUnk))
FUNCTION(D2Common, -10493,  DWORD, __stdcall, CheckCollision, (Room1* pRoom, DWORD dwX, DWORD dwY, DWORD dwBitMask))
FUNCTION(D2Common, -10445,  DWORD, __stdcall, GetUnitMaxHP, (UnitAny* pUnit))
FUNCTION(D2Common, -10099,  DWORD, __stdcall, CheckUnitSlot, (Inventory* pInventory, DWORD dwSlot))

FUNCTION(D2Net, -10036, void, __stdcall, SendPacket, (size_t aLen, DWORD arg1, BYTE* aPacket))
FUNCTION(D2Net, 0x6510, void, __stdcall, ReceivePacket, (BYTE *aPacket, DWORD aLen))
FUNCTION(D2Net, -10002, void, __stdcall, ReceivePacket_I, (BYTE *aPacket, DWORD aLen))

FUNCTION(D2Gfx, -10061, void, __stdcall, DrawAutomapCell, (CellContext *context, DWORD xpos, DWORD ypos, RECT *cliprect, DWORD bright))
FUNCTION(D2Gfx, -10024, void, __stdcall, DrawAutomapCell2, (CellContext *context, DWORD xpos, DWORD ypos, DWORD bright2, DWORD bright, BYTE *coltab)) 
FUNCTION(D2Gfx, 0x7D10, void, __stdcall, DrawRectangle, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwTrans))
FUNCTION(D2Gfx, 0x7CA0, void, __stdcall, DrawLine, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk))
FUNCTION(D2Gfx, -10043, DWORD, __stdcall, GetScreenSize, (void))
FUNCTION(D2Gfx, -10078, HWND, __stdcall, GetHwnd, (void))
FUNCTION(D2Gfx, -10026, void, __fastcall, WindowCleanup, (void))

FUNCTION(D2Multi, 0xF8C0, void, __stdcall, PrintChannelText, (const char* szText, DWORD dwColor)) // Updated.
FUNCTION(D2Multi, 0xFE50, void, __stdcall, SayChannelText, (const char * szText)) // Clientside
FUNCTION(D2Multi, 0xC650, void, __fastcall, DoChat, (void))
FUNCTION(D2Multi, 0xFC44, void, __fastcall, WhisperIntercept_Jump, (void))
FUNCTION(D2Cmp, -10006, void, __stdcall, InitCellFile, (void *cellfile, CellFile **outptr, char *srcfile, DWORD lineno, DWORD filever, char *filename))
FUNCTION(D2Cmp, -10106, void, __stdcall, DeleteCellFile, (CellFile *cellfile)) 

FUNCTION(D2Lang, -10005, wchar_t*, __fastcall, GetLocaleText, (WORD nLocaleTxtNo))

FUNCTION(D2Win, 0xD680, void, __fastcall, DrawSprites, (void))
FUNCTION(D2Win, -10095, void*, __fastcall, SetControlText, (Control* box, wchar_t* txt))
FUNCTION(D2Win, -10096, DWORD, __fastcall, GetTextWidthFileNo, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo))
FUNCTION(D2Win, -10010, DWORD, __fastcall, SetTextSize, (DWORD dwSize))
FUNCTION(D2Win, -10001, void, __fastcall, DrawGameString, (wchar_t *wStr, int xPos, int yPos, DWORD dwColor, DWORD dwUnk))
FUNCTION(D2Win, -10121, void,__fastcall, ClientCleanup,(void))

#endif

