#pragma once
#ifndef __D2PTRS_H__
#define __D2PTRS_H__

#include "D2Structs.h"

#pragma warning ( push )
#pragma warning ( disable: 4245 )

#pragma optimize ( "", off )

#ifdef _DEFINE_VARS

enum {DLLNO_D2CLIENT, DLLNO_D2COMMON, DLLNO_D2GFX, DLLNO_D2LANG, DLLNO_D2WIN, DLLNO_D2NET, DLLNO_D2GAME, DLLNO_D2LAUNCH, DLLNO_FOG, DLLNO_BNCLIENT, DLLNO_STORM, DLLNO_D2CMP, DLLNO_D2MULTI};

#define DLLOFFSET(a1,b1) ((DLLNO_##a1)|((b1)<<8))
#define FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; d1##_##v1##_t *d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    d1##_##v1##_t *p_##d1##_##v1 = (d1##_##v1##_t *)DLLOFFSET(d1,o1);
#define ASMPTR(d1,v1,o1)			DWORD d1##_##v1 = DLLOFFSET(d1,o1);

#else

#define FUNCPTR(d1,v1,t1,t2,o1)	typedef t1 d1##_##v1##_t t2; extern d1##_##v1##_t *d1##_##v1;
#define VARPTR(d1,v1,t1,o1)		typedef t1 d1##_##v1##_t;    extern d1##_##v1##_t *p_##d1##_##v1;
#define ASMPTR(d1,v1,o1)			extern DWORD d1##_##v1;

#endif
#define _D2PTRS_START	D2CLIENT_GetQuestInfo

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2CLIENT, GetQuestInfo, void* __stdcall, (void), 0x17D00)
FUNCPTR(D2CLIENT, SubmitItem, void __fastcall, (DWORD dwItemId), 0x18410)
FUNCPTR(D2CLIENT, GetCurrentInteractingNPC, UnitAny* __fastcall, (void), 0x18450)
FUNCPTR(D2CLIENT, ShopAction, void __fastcall, (UnitAny* pItem, UnitAny* pNpc, UnitAny* pNpc2, DWORD dwSell, DWORD dwItemCost, DWORD dwMode, DWORD _2, DWORD _3), 0x19E00)
FUNCPTR(D2CLIENT, CloseNPCInteract, void __fastcall, (void), 0x1A630)
//FUNCPTR(D2CLIENT, ClearScreen, void __fastcall, (void), 0x1B4C0) // unused but I want to look into using it
FUNCPTR(D2CLIENT, FindClientSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x1F1A0)
FUNCPTR(D2CLIENT, FindServerSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x1F1C0)
//FUNCPTR(D2CLIENT, UnitTestSelect, DWORD __stdcall, (UnitAny* pUnit, DWORD _1, DWORD _2, DWORD _3), 0x20010) // unused but we need to use it
FUNCPTR(D2CLIENT, GetAutomapSize, DWORD __stdcall, (), 0x3CFC0)
FUNCPTR(D2CLIENT, NewAutomapCell, AutomapCell * __fastcall, (), 0x3D5B0)
FUNCPTR(D2CLIENT, AddAutomapCell, void __fastcall, (AutomapCell *aCell, AutomapCell **node), 0x3F090)
FUNCPTR(D2CLIENT, RevealAutomapRoom, void __stdcall, (Room1 *pRoom1, DWORD dwClipFlag, AutomapLayer *aLayer), 0x404C0)
FUNCPTR(D2CLIENT, InitAutomapLayer_I, AutomapLayer* __fastcall, (DWORD nLayerNo), 0x40650)
FUNCPTR(D2CLIENT, CancelTrade, void __fastcall, (void), 0x56440)
FUNCPTR(D2CLIENT, ClickMap, void __stdcall, (DWORD MouseFlag, DWORD x, DWORD y, DWORD Type), 0x5CE80)
FUNCPTR(D2CLIENT, GetMouseXOffset, DWORD __fastcall, (void), 0x5D670)
FUNCPTR(D2CLIENT, GetMouseYOffset, DWORD __fastcall, (void), 0x5D680)
FUNCPTR(D2CLIENT, SetSelectedUnit_I, void __fastcall, (UnitAny *pUnit), 0x6EA80)
FUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny * __stdcall, (), 0x6ECA0)
//FUNCPTR(D2CLIENT, PrintPartyString, void __stdcall, (wchar_t *wMessage, int nColor), 0x71500) // unused but I want to look into using it too
FUNCPTR(D2CLIENT, PrintGameString, void __stdcall, (wchar_t *wMessage, int nColor), 0x71740)
FUNCPTR(D2CLIENT, LeaveParty, void __fastcall, (void), 0x79160)
FUNCPTR(D2CLIENT, GetDifficulty, BYTE __stdcall, (), 0x79CD0)
FUNCPTR(D2CLIENT, ExitGame, void __fastcall, (void), 0x7AB00)
FUNCPTR(D2CLIENT, CloseInteract, void __fastcall, (void), 0x7BBD0)
FUNCPTR(D2CLIENT, Transmute, void __fastcall, (void), 0xA6560)
FUNCPTR(D2CLIENT, GetUiVar_I, DWORD __fastcall, (DWORD dwVarNo), 0x8AA90)
FUNCPTR(D2CLIENT, DrawRectFrame, void __fastcall, (DWORD Rect), 0x8AB50)
FUNCPTR(D2CLIENT, PerformGoldDialogAction, void __fastcall, (void), 0x8C1F0)
FUNCPTR(D2CLIENT, GetCursorItem, UnitAny* __fastcall, (void), 0x9EB40)
FUNCPTR(D2CLIENT, InitInventory, void __fastcall, (void), 0xA8B70)
FUNCPTR(D2CLIENT, GetItemName, BOOL __stdcall, (UnitAny* pItem, wchar_t* wBuffer, DWORD dwSize), 0xAB3C0)
FUNCPTR(D2CLIENT, LeftClickItem, void __stdcall, (UnitAny* pPlayer, Inventory* pInventory, INT x, INT y, DWORD dwClickType, InventoryLayout* pLayout, DWORD Location), 0xB0900)
FUNCPTR(D2CLIENT, GetMercUnit, UnitAny* __fastcall, (void), 0xB1B30)
FUNCPTR(D2CLIENT, AcceptTrade, void __fastcall, (void), 0xC2610)
FUNCPTR(D2CLIENT, GetMonsterOwner, DWORD __fastcall, (DWORD nMonsterId), 0xC3160)
FUNCPTR(D2CLIENT, GetUnitHPPercent, DWORD __fastcall, (DWORD dwUnitId), 0xC3040)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2CLIENT, ScreenSizeX, DWORD, 0xDC6E0)
VARPTR(D2CLIENT, ScreenSizeY, DWORD, 0xDC6E4)
VARPTR(D2CLIENT, CursorHoverX, DWORD, 0xDCAD8)
VARPTR(D2CLIENT, CursorHoverY, DWORD, 0xDCADC)
VARPTR(D2CLIENT, Divisor, int, 0xF3B98)
VARPTR(D2CLIENT, MercStrIndex, WORD, 0xF6B4C)
VARPTR(D2CLIENT, NPCMenu, NPCMenu*, 0xF8300)
VARPTR(D2CLIENT, NPCMenuAmount, DWORD, 0xF8A50)
VARPTR(D2CLIENT, TradeLayout, InventoryLayout*, 0x100B78)
VARPTR(D2CLIENT, StashLayout, InventoryLayout*, 0x100BC0)
VARPTR(D2CLIENT, StoreLayout, InventoryLayout*, 0x100CA0)
VARPTR(D2CLIENT, CubeLayout, InventoryLayout*, 0x100CB8)
VARPTR(D2CLIENT, InventoryLayout, InventoryLayout*, 0x100CD0)
VARPTR(D2CLIENT, MouseX, DWORD, 0x101638)
VARPTR(D2CLIENT, MouseY, DWORD, 0x101634)
VARPTR(D2CLIENT, RegularCursorType, DWORD, 0x101674)
VARPTR(D2CLIENT, GoldDialogAction, DWORD, 0x102B38)
VARPTR(D2CLIENT, AutomapOn, DWORD, 0x102B80)
VARPTR(D2CLIENT, Ping, DWORD, 0x1032BC)
VARPTR(D2CLIENT, OverheadTrigger, DWORD, 0x1054F6)
VARPTR(D2CLIENT, MouseOffsetY, int, 0x1121CC)
VARPTR(D2CLIENT, MouseOffsetX, int, 0x1121D0)
VARPTR(D2CLIENT, RecentInteractId, DWORD, 0x11B625)
VARPTR(D2CLIENT, ItemPriceList, DWORD, 0x11B643)
VARPTR(D2CLIENT, TransactionDialog, void*, 0x11B663)
VARPTR(D2CLIENT, GameInfo, GameStructInfo *, 0x11B908)
VARPTR(D2CLIENT, WaypointTable, DWORD, 0x11B951)
VARPTR(D2CLIENT, ShopCursorType, DWORD, 0x11B9F8)
VARPTR(D2CLIENT, SelectedInvItem, UnitAny*, 0x11B9FC)
VARPTR(D2CLIENT, bWeapSwitch, DWORD, 0x11BA58)
VARPTR(D2CLIENT, MercLayout, InventoryLayout*, 0x11BB5C)
VARPTR(D2CLIENT, bTradeAccepted, DWORD, 0x11BC2C)
VARPTR(D2CLIENT, bTradeBlock, DWORD, 0x11BC3C)
VARPTR(D2CLIENT, GoldDialogAmount, DWORD, 0x11BF38)
VARPTR(D2CLIENT, ExpCharFlag, DWORD, 0x10330C)
//VARPTR(D2CLIENT, MapId, DWORD, 0x11C020) // unused but I want to add it
VARPTR(D2CLIENT, AlwaysRun, DWORD, 0x11C050)
VARPTR(D2CLIENT, PlayerUnitList, RosterUnit*, 0x11C080)
//VARPTR(D2CLIENT, NoPickUp, DWORD, 0x11C114) // unused but I want to add it
VARPTR(D2CLIENT, TransactionDialogs_2, DWORD, 0x11C190)
VARPTR(D2CLIENT, TransactionDialogs, DWORD, 0x11C194)
//VARPTR(D2CLIENT, ScreenCovered, DWORD, 0x11C1D0) // unused and I wonder what it is
//VARPTR(D2CLIENT, SelectedUnit, UnitAny*, 0x11C1D8) // unused, but can we use it somewhere maybe?
VARPTR(D2CLIENT, RecentTradeId, DWORD, 0x11C1F4)
VARPTR(D2CLIENT, Offset, POINT, 0x11C2E8)
VARPTR(D2CLIENT, AutomapLayer, AutomapLayer*, 0x11C2B4)
VARPTR(D2CLIENT, FPS, DWORD, 0x11C388)
VARPTR(D2CLIENT, PlayerUnit, UnitAny*, 0x11C3D0)
VARPTR(D2CLIENT, MercReviveCost, DWORD, 0x11C3D4)
VARPTR(D2CLIENT, ChatMsg, wchar_t*, 0x11FC40)
VARPTR(D2CLIENT, RecentTradeName, wchar_t*, 0x12334C)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Client Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2CLIENT, TakeWaypoint_I, 0x112F3)
ASMPTR(D2CLIENT, ClickShopItem_I, 0x191C0)
ASMPTR(D2CLIENT, ShopAction_I, 0x19E00)
ASMPTR(D2CLIENT, GetUnitName_I, 0x1F3D0)
ASMPTR(D2CLIENT, AssignPlayer_I, 0x20D60)
ASMPTR(D2CLIENT, TestPvpFlag_I, 0x30DD0)
ASMPTR(D2CLIENT, PlaySound_I, 0x31FA0)
ASMPTR(D2CLIENT, Interact_I, 0x5CDD0)
ASMPTR(D2CLIENT, ClickParty_II, 0x62400)
ASMPTR(D2CLIENT, ClickParty_I, 0x78D10)
ASMPTR(D2CLIENT, InputCall_I, 0x79670)
ASMPTR(D2CLIENT, TakeWP_I, 0x797E0)
ASMPTR(D2CLIENT, MercItemAction_I, 0x798E0)
ASMPTR(D2CLIENT, ClickBelt_I, 0x81FB0)
ASMPTR(D2CLIENT, ClickBeltRight_I, 0x83DF0)
ASMPTR(D2CLIENT, Say_I, 0x87206)
ASMPTR(D2CLIENT, GetItemDesc_I, 0x8A140)
ASMPTR(D2CLIENT, BodyClickTable, 0xDCAE4)
ASMPTR(D2CLIENT, ClickItemRight_I, 0xB60F0)
ASMPTR(D2CLIENT, LoadUIImage_I, 0xBEED0)
ASMPTR(D2CLIENT, GetMinionCount_I, 0xC3340)
ASMPTR(D2CLIENT, GameLeave_I, 0x7E0F0)

ASMPTR(D2CLIENT, ClickMap_I, 0x102B7C)
ASMPTR(D2CLIENT, ClickMap_II, 0x11C1D8)
ASMPTR(D2CLIENT, ClickMap_III, 0x5D5A0)
ASMPTR(D2CLIENT, ClickMap_IV, 0x5CEF9)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2COMMON, GetUnitStat, DWORD __stdcall, (UnitAny* pUnit, DWORD dwStat, DWORD dwStat2), 0x34E20)
FUNCPTR(D2COMMON, GetObjectText, ObjectTxt * __stdcall, (DWORD objno), 0x37300)
FUNCPTR(D2COMMON, GetItemText, ItemTxt *__stdcall, (DWORD dwItemNo), 0x5C980)
FUNCPTR(D2COMMON, GetLayer, AutomapLayer2* __fastcall, (DWORD dwLevelNo), 0x73870)
FUNCPTR(D2COMMON, GetLevelText, LevelTxt * __stdcall, (DWORD levelno), 0x73A10)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2COMMON, GetRepairCost, DWORD __stdcall, (DWORD _1, UnitAny* pUnit, DWORD dwNpcId, DWORD dwDifficulty, DWORD dwItemPriceList, DWORD _2), -10027)
FUNCPTR(D2COMMON, GenerateOverheadMsg, OverheadMsg* __stdcall, (DWORD dwUnk, char* szMsg, DWORD dwTrigger), -10041)
FUNCPTR(D2COMMON, GetItemLevelRequirement, DWORD __stdcall, (UnitAny* pItem, UnitAny* pPlayer), -10113)
FUNCPTR(D2COMMON, GetItemPrice, DWORD __stdcall, (UnitAny* MyUnit, UnitAny* pItem, DWORD U1_,DWORD U2_,DWORD U3_,DWORD U4_), -10122)
FUNCPTR(D2COMMON, AddRoomData, void __stdcall, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom), -10184)
FUNCPTR(D2COMMON, CheckUnitCollision, DWORD __stdcall, (UnitAny* pUnitA, UnitAny* pUnitB, DWORD dwBitMask), -10233)
FUNCPTR(D2COMMON, GetUnitState, int __stdcall, (UnitAny *pUnit, DWORD dwStateNo), -10295)
FUNCPTR(D2COMMON, FixOverheadMsg, void __stdcall, (OverheadMsg* pMsg, DWORD dwUnk), -10315)
FUNCPTR(D2COMMON, GetRoomFromUnit,  Room1* __stdcall, (UnitAny * ptUnit), -10366)
FUNCPTR(D2COMMON, CopyStatList, DWORD __stdcall, (StatList* pStatList, Stat* pStatArray, DWORD dwMaxEntries), -10373)
FUNCPTR(D2COMMON, GetQuestFlag, INT __stdcall, (void* QuestInfo, DWORD dwAct, DWORD dwQuest), -10600)
FUNCPTR(D2COMMON, GetStatList, StatList* __stdcall, (UnitAny* pUnit, DWORD dwUnk, DWORD dwMaxEntries ), -10611)
FUNCPTR(D2COMMON, AbsScreenToMap, void __stdcall, (long *pX, long *pY), -10725)
FUNCPTR(D2COMMON, GetNextItemFromInventory, UnitAny *__stdcall, (UnitAny *pItem), -10770)
FUNCPTR(D2COMMON, CheckWaypoint, DWORD __stdcall, (DWORD WaypointTable, DWORD dwLevelId), -10875)
FUNCPTR(D2COMMON, GetSkillLevel, INT __stdcall, (UnitAny* pUnit, Skill* pSkill, BOOL bTotal), -10904)
FUNCPTR(D2COMMON, MapToAbsScreen, void __stdcall, (long *pX, long *pY), -10912)
FUNCPTR(D2COMMON, RemoveRoomData, void __stdcall, (Act* ptAct, int LevelId, int Xpos, int Ypos, Room1* pRoom), -11009)
FUNCPTR(D2COMMON, GetLevel, Level * __fastcall, (ActMisc *pMisc, DWORD dwLevelNo), -11020)
FUNCPTR(D2COMMON, GetItemMagicalMods, char* __stdcall, (WORD wPrefixNum), -11065)
FUNCPTR(D2COMMON, GetItemFromInventory, UnitAny *__stdcall, (Inventory* inv), -11151)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2COMMON, sqptDataTable, DWORD, 0x9B500)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Common Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2COMMON, DisplayOverheadMsg_I, -10375)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2NET, ReceivePacket_I, void __stdcall, (BYTE *aPacket, DWORD aLen), -10002)
FUNCPTR(D2NET, SendPacket, void __stdcall, (size_t aLen, DWORD arg1, BYTE* aPacket), -10036)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Globals
////////////////////////////////////////////////////////////////////////////////////////////////

//VARPTR(D2NET, CriticalPacketSection, CRITICAL_SECTION, 0xB400) // unused but we need to use it


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Gfx Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwTrans), 0x7D10)
FUNCPTR(D2GFX, DrawLine, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0x7CA0)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Net Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2GFX, DrawAutomapCell2, void __stdcall, (CellContext* context, DWORD xpos, DWORD ypos, DWORD bright2, DWORD bright, BYTE *coltab), -10024)
FUNCPTR(D2GFX, GetScreenSize, DWORD __stdcall, (), -10043)
FUNCPTR(D2GFX, GetHwnd, HWND __stdcall, (), -10078)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2MULTI, DoChat, void __fastcall, (void), 0xC650)
FUNCPTR(D2MULTI, PrintChannelText, void __stdcall, (char *szText, DWORD dwColor),  0xF8C0)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Multi Globals
////////////////////////////////////////////////////////////////////////////////////////////////
VARPTR(D2MULTI, ChatBoxMsg, char*, 0x38F18)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Cmp Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2CMP, InitCellFile, void __stdcall, (void *cellfile, CellFile **outptr, char *srcfile, DWORD lineno, DWORD filever, char *filename), -10006)
FUNCPTR(D2CMP, DeleteCellFile, void __stdcall, (CellFile *cellfile), -10106)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Lang Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2LANG, GetLocaleText, wchar_t* __fastcall, (WORD nLocaleTxtNo), -10005)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Lang Stubs
////////////////////////////////////////////////////////////////////////////////////////////////

ASMPTR(D2LANG, Say_II, 0xA6E0)


////////////////////////////////////////////////////////////////////////////////////////////////
// D2Launch Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2LAUNCH, BnData, BnetData *, 0x25ACC)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2WIN, DrawSprites, void __fastcall, (void), 0xD680)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Ordinals
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2WIN, DrawText, void __fastcall, (wchar_t *wStr, int xPos, int yPos, DWORD dwColor, DWORD dwUnk), -10001)
FUNCPTR(D2WIN, SetTextSize, DWORD __fastcall, (DWORD dwSize), -10010)
FUNCPTR(D2WIN, SetControlText, void* __fastcall, (Control* box, wchar_t* txt), -10095)
FUNCPTR(D2WIN, GetTextWidthFileNo, DWORD __fastcall, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo), -10096)

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Win Globals
////////////////////////////////////////////////////////////////////////////////////////////////

VARPTR(D2WIN, FirstControl, Control *, 0x5C718)
//VARPTR(D2WIN, FocusedControl, Control*, 0x5C728) // unused, but we ought to use it

////////////////////////////////////////////////////////////////////////////////////////////////
// D2Game Functions
////////////////////////////////////////////////////////////////////////////////////////////////

FUNCPTR(D2GAME, Rand, DWORD __fastcall, (DWORD* seed), 0x1080)

#define _D2PTRS_END	D2GAME_Rand

#undef FUNCPTR
#undef VARPTR
#undef ASMPTR

#define p_D2CLIENT_MyPlayerUnit				(*p_D2CLIENT_PlayerUnit)
#define GetItemFlag(Unit, Flag)				(D2COMMON_GetItemFlag(Unit, Flag, 0, "?"))
#define D2CLIENT_TestPvpFlag(dwId1, dwId2, dwFlag)	(TestPvpFlag_STUB(dwId1, dwId2, dwFlag))
#define GetUnitStat(unit, stat)				(D2COMMON_GetUnitStat(unit, stat, 0))
#define GetBaseStat(unit, stat)				(D2COMMON_GetBaseStat(unit, stat, 0))
#define D2CLIENT_GetUIState(dwVarNo)		(D2CLIENT_GetUIVar_STUB(dwVarNo))
#define D2CLIENT_InitAutomapLayer(layerlvl)	((AutomapLayer*)D2CLIENT_InitAutomapLayer_STUB(layerlvl))
#define D2CLIENT_GetUnitName(x)				(wchar_t*)D2CLIENT_GetUnitName_STUB((DWORD)x)
#define D2CLIENT_SetSelectedUnit(x)			(D2CLIENT_SetSelectedUnit_STUB((DWORD)x))
#define D2CLIENT_LoadUIImage(x)				((CellFile*)D2CLIENT_LoadUIImage_ASM(x))
#define D2CLIENT_Interact_(x)				(D2CLIENT_Interact_ASM((DWORD)x))
#define D2CLIENT_clickParty(x,y)			(D2CLIENT_clickParty_ASM((DWORD)x, (DWORD)y))
#define D2CLIENT_ClickShopItem(x,y,z)		(D2CLIENT_ClickShopItem_ASM(x,y,z))
#define D2CLIENT_RightClickItem(X,Y,LOCATION, PLAYER, INVENTORYDATA) D2CLIENT_clickItemRight_ASM(X,Y,LOCATION, (DWORD)PLAYER, (DWORD)INVENTORYDATA)
#define D2CLIENT_clickBeltRight(pPlayer, pInventory, dwShift, dwPotPos)	D2CLIENT_clickBeltRight_ASM((DWORD)pPlayer, (DWORD)pInventory, dwShift, dwPotPos)
#define D2CLIENT_GetItemDesc(pUnit, pBuffer) D2CLIENT_GetItemDesc_ASM((DWORD)pUnit, pBuffer)
#define D2COMMON_DisplayOverheadMsg(pUnit)			D2COMMON_DisplayOverheadMsg_ASM((DWORD)pUnit)
#define D2CLIENT_MercItemAction(bPacketType, dwSlotId)	D2CLIENT_MercItemAction_ASM(bPacketType, dwSlotId)
#define D2CLIENT_PickItem(Unit)				(D2CLIENT_PickItem_ASM((DWORD)Unit))
#define D2WIN_GetHwnd						D2GFX_GetHwnd
#define D2GFX_DrawFrame(Rect)				DrawRectFrame_STUB(Rect)

#pragma warning ( pop )

#pragma optimize ( "", on )

#endif
