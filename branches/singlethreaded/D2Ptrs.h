#include "D2Structs.h"

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
#define _D2PTRS_START	D2CLIENT_GetMonsterTxt

FUNCPTR(D2CLIENT, GetMonsterTxt, MonsterTxt * __fastcall, (DWORD monno), 0x1390) // Updated
FUNCPTR(D2CLIENT, PrintGameString, void __stdcall, (wchar_t *wMessage, int nColor), 0x71740) // Updated
FUNCPTR(D2CLIENT, PrintPartyString, void __stdcall, (wchar_t *wMessage, int nColor), 0x71500) // Updated
FUNCPTR(D2CLIENT, PrintGameString2, void __stdcall, (char *szMessage), 0x73EE0) // Updated
FUNCPTR(D2CLIENT, GetPlayerXOffset, int __stdcall, (), 0x5D650) // Updated
FUNCPTR(D2CLIENT, GetPlayerYOffset, int __stdcall, (), 0x5D670) // Updated
FUNCPTR(D2CLIENT, SetSelectedUnit_I, void __fastcall, (UnitAny *pUnit), 0x6EA80) // Updated
FUNCPTR(D2CLIENT, GetSelectedUnit, UnitAny * __stdcall, (), 0x6ECA0) // Updated
FUNCPTR(D2CLIENT, GetDifficulty, BYTE __stdcall, (), 0x79CD0) // Updated
FUNCPTR(D2CLIENT, GetUnknownFlag, DWORD __fastcall, (), 0x30080) // Updated
FUNCPTR(D2CLIENT, GetGameInfo, GameStructInfo *__stdcall, (), 0x120F0) // Updated
FUNCPTR(D2CLIENT, GetAutomapSize, DWORD __stdcall, (), 0x3CFC0) // Updated
FUNCPTR(D2CLIENT, NewAutomapCell, AutomapCell * __fastcall, (), 0x3D5B0) // Updated
FUNCPTR(D2CLIENT, AddAutomapCell, void __fastcall, (AutomapCell *aCell, AutomapCell **node), 0x3F090) // Updated
FUNCPTR(D2CLIENT, RevealAutomapRoom, void __stdcall, (Room1 *pRoom1, DWORD dwClipFlag, AutomapLayer *aLayer), 0x404C0) // Updated
FUNCPTR(D2CLIENT, InitAutomapLayer_I, AutomapLayer* __fastcall, (DWORD nLayerNo), 0x40650) // Updated
FUNCPTR(D2CLIENT, GetMonsterOwner, DWORD __fastcall, (DWORD nMonsterId), 0xC3160) // Updated
FUNCPTR(D2CLIENT, GetUiVar_I, DWORD __fastcall, (DWORD dwVarNo), 0x8AA90) // Updated
FUNCPTR(D2CLIENT, SetUIState, DWORD __fastcall, (DWORD varno, DWORD howset, DWORD unknown1), 0x8EF00) // Updated
FUNCPTR(D2CLIENT, GetItemNameString, void __stdcall, (UnitAny *pItem, wchar_t *wItemName, int nLen), 0xAB3C0) // Update
FUNCPTR(D2CLIENT, CalculateShake, void __stdcall, (DWORD *dwPosX, DWORD *dwPosY), 0xBB4F0) // Update
FUNCPTR(D2CLIENT, GetPlayerUnit, UnitAny*  __stdcall,(),0x1E490) // Update
FUNCPTR(D2CLIENT, InitLevel_I, AutomapLayer* __fastcall,(),0x40650) // Update 
FUNCPTR(D2CLIENT, DrawRectFrame, VOID __fastcall, (DWORD Rect), 0x8AB50) // Update
FUNCPTR(D2CLIENT, ExitGame, VOID __fastcall, (VOID), 0x7AB00) // Update
FUNCPTR(D2CLIENT, Attack, VOID __stdcall, (AttackStruct* Attack, BOOL AttackingUnit), 0x5B050) // Update
FUNCPTR(D2CLIENT, GetItemName, BOOL __stdcall, (UnitAny* pItem, wchar_t* wBuffer, DWORD dwSize), 0xAB3C0) // Update
FUNCPTR(D2CLIENT, AcceptTrade, VOID __fastcall, (VOID), 0xC2610) // Update
FUNCPTR(D2CLIENT, CancelTrade, VOID __fastcall, (VOID), 0x56440) // Update
FUNCPTR(D2CLIENT, GetMouseXOffset, DWORD __fastcall, (VOID), 0x5D670) // Updated
FUNCPTR(D2CLIENT, GetMouseYOffset, DWORD __fastcall, (VOID), 0x5D680) // Updated
FUNCPTR(D2CLIENT, ClearScreen, VOID __fastcall, (VOID), 0x1B4C0) // Updated
FUNCPTR(D2CLIENT, GetQuestInfo, VOID* __stdcall, (VOID), 0x17D00) // Updated
FUNCPTR(D2CLIENT, UnitTestSelect, DWORD __stdcall, (UnitAny* pUnit, DWORD _1, DWORD _2, DWORD _3), 0x20010) // Updated
FUNCPTR(D2CLIENT, FindServerSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x1F1C0) // Updated
FUNCPTR(D2CLIENT, FindClientSideUnit, UnitAny* __fastcall, (DWORD dwId, DWORD dwType), 0x1F1A0) // Updated
FUNCPTR(D2CLIENT, SetUIVar, DWORD __fastcall, (DWORD varno, DWORD howset, DWORD unknown1), 0x8EF00) // Updated
FUNCPTR(D2CLIENT, clickMap, VOID __stdcall, (DWORD MouseFlag, DWORD x, DWORD y, DWORD Type), 0x5CE80) // Updated
FUNCPTR(D2CLIENT, GetCursorItem, UnitAny* __fastcall, (VOID), 0x9EB40) // Updated
FUNCPTR(D2CLIENT, LeftClickItem, VOID __stdcall, (UnitAny* pPlayer, Inventory* pInventory, INT x, INT y, DWORD dwClickType, InventoryLayout* pLayout, DWORD Location), 0xB0900) // Updated
FUNCPTR(D2CLIENT, CloseNPCInteract, VOID __fastcall, (VOID), 0x1A630) // Updated 
FUNCPTR(D2CLIENT, CloseInteract, VOID __fastcall, (VOID), 0x7BBD0) // Updated
FUNCPTR(D2CLIENT, ChatBoxHandler, DWORD __stdcall, (MSG* pMsg), 0x86F80) // Updated
FUNCPTR(D2CLIENT, InitInventory, VOID __fastcall, (VOID), 0xA8B70) // Updated
FUNCPTR(D2CLIENT, FixShopActions, VOID __fastcall, (VOID), 0x19D90) // Updated
FUNCPTR(D2CLIENT, submitItem, VOID __fastcall, (DWORD dwItemId), 0x18410) // Updated
FUNCPTR(D2CLIENT, GetUnitHPPercent, DWORD __fastcall, (DWORD dwUnitId), 0xC3040) // Updated
FUNCPTR(D2CLIENT, GetMercUnit, UnitAny* __fastcall, (VOID), 0xB1B30) // Updated
FUNCPTR(D2CLIENT, ShopAction, VOID __fastcall, (UnitAny* pItem, UnitAny* pNpc, UnitAny* pNpc2, DWORD dwSell, DWORD dwItemCost, DWORD dwMode, DWORD _2, DWORD _3), 0x19E00) // Updated
FUNCPTR(D2CLIENT, GetCurrentInteractingNPC, UnitAny* __fastcall, (VOID), 0x18450) // Updated
FUNCPTR(D2CLIENT, PerformGoldDialogAction, VOID __fastcall, (VOID), 0x8C1F0)
FUNCPTR(D2CLIENT, LeaveParty, VOID __fastcall, (VOID), 0x79160)
FUNCPTR(D2CLIENT, Transmute, VOID __fastcall, (VOID), 0xA6560)

VARPTR(D2CLIENT, MouseOffsetY, int, 0x1121CC) // Updated
VARPTR(D2CLIENT, MouseOffsetX, int, 0x1121D0) // Updated
VARPTR(D2CLIENT, Divisor, int, 0xF3B98) // Updated
VARPTR(D2CLIENT, yShake, int, 0xFD114) // Updated
VARPTR(D2CLIENT, GameInfo, GameStructInfo *, 0x11B908); // Updated
VARPTR(D2CLIENT, Ping, DWORD, 0x1032BC) // Updated
VARPTR(D2CLIENT, FPS, DWORD, 0x11C388) // Updated
VARPTR(D2CLIENT, WaypointTab, DWORD, 0x11B956) // Updated
VARPTR(D2CLIENT, xShake, int, 0x11B9B4) // Updated
VARPTR(D2CLIENT, SelectedInvItem, UnitAny*, 0x11B9FC) // Updated
VARPTR(D2CLIENT, Offset, POINT, 0x11C2E8) // Updated
VARPTR(D2CLIENT, AutomapOn, DWORD, 0x102B80)
VARPTR(D2CLIENT, FirstAutomapLayer, AutomapLayer *, 0x11C2B0) // Updated
VARPTR(D2CLIENT, AutomapLayer, AutomapLayer *, 0x11C2B4) // Updated
VARPTR(D2CLIENT, AutomapYPosition, int, 0x11C30C) // Updated
VARPTR(D2CLIENT, PlayerUnit, UnitAny *, 0x11C3D0) // Updated
VARPTR(D2CLIENT, PlayerUnitList, RosterUnit *, 0x11C080) // Updated
VARPTR(D2CLIENT, QuestTab, DWORD, 0x11D50D) // Updated
VARPTR(D2CLIENT, MouseX, DWORD, 0x101638)// Updated
VARPTR(D2CLIENT, MouseY, DWORD, 0x101638-4) // Updated
VARPTR(D2CLIENT, MapId, DWORD, 0x11C020)// Updated
VARPTR(D2CLIENT, bWeapSwitch, DWORD, 0x11BA58) // Updated
VARPTR(D2CLIENT, WaypointTable, DWORD, 0x11B951) // Updated
VARPTR(D2CLIENT, bTradeAccepted, DWORD, 0x11BC2C) // Updated
VARPTR(D2CLIENT, bTradeBlock, DWORD, 0x11BC3C) // Updated
VARPTR(D2CLIENT, TradeFlag, DWORD, 0x11C1F4) // Updated
VARPTR(D2CLIENT, ScreenSizeX, DWORD, 0xDC6E0) // Updated
VARPTR(D2CLIENT, ScreenSizeY, DWORD, 0xDC6E0+4) // Updated
VARPTR(D2CLIENT, ChatMsg, wchar_t*, 0x11FC40) // Updated
VARPTR(D2CLIENT, AlwaysRun, DWORD, 0x11C050) // Updated
VARPTR(D2CLIENT, RecentTradeName, wchar_t*, 0x12334C) // Updated
VARPTR(D2CLIENT, RecentTradeId, DWORD, 0x11C1F4) // Updated
VARPTR(D2CLIENT, SelectedUnit, UnitAny*, 0x11C1D8) // Updated
VARPTR(D2CLIENT, ItemPriceList, DWORD, 0x11B643) // Updated
VARPTR(D2CLIENT, RepairCosts, DWORD, 0x11B67B) // Updated
VARPTR(D2CLIENT, RecentInteractId, DWORD, 0x11B625) // Updated
VARPTR(D2CLIENT, NPCMenu, NPCMenu*, 0xF8300) // Updated
VARPTR(D2CLIENT, NPCMenuAmount, DWORD, 0xF8A50) // Updated
VARPTR(D2CLIENT, ScreenCovered, DWORD, 0x11C1D0) // Updated
VARPTR(D2CLIENT, InventoryLayout, InventoryLayout*, 0x100CD0) // Updated
VARPTR(D2CLIENT, StashLayout, InventoryLayout*, 0x100BC0) // Updated 100BC0 100BC0
VARPTR(D2CLIENT, StoreLayout, InventoryLayout*, 0x100CA0) // Updated
VARPTR(D2CLIENT, CubeLayout, InventoryLayout*, 0x100CB8) // Updated
VARPTR(D2CLIENT, TradeLayout, InventoryLayout*, 0x100B78) // Updated
VARPTR(D2CLIENT, MercLayout, InventoryLayout*, 0x11BB5C) // Updated
VARPTR(D2CLIENT, StoreTable, DWORD, 0x101638) // Updated
VARPTR(D2CLIENT, ChatTextBuffer, wchar_t*, 0x11FC40) // Updated
VARPTR(D2CLIENT, RegularCursorType, DWORD, 0x101674) // Updated
VARPTR(D2CLIENT, ShopCursorType, DWORD, 0x11B9F8) // Updated
VARPTR(D2CLIENT, OverheadTrigger, DWORD, 0x1054F6) // Updated
VARPTR(D2CLIENT, ExpCharFlag, DWORD, 0x10330C) // Updated
VARPTR(D2CLIENT, ChatBoxFlag, DWORD, 0x11BF4C) // Updated
VARPTR(D2CLIENT, MercStrIndex, WORD, 0xF6B4C) // Updated
VARPTR(D2CLIENT, MercReviveCost, DWORD, 0x11C3D4) // Updated
VARPTR(D2CLIENT, ShopPerformFlag_I, DWORD, 0x11B56B) // Updated
VARPTR(D2CLIENT, ShopPerformFlag_II, DWORD, 0x11B667) // Updated
VARPTR(D2CLIENT, GoldDialogAmount, DWORD, 0x11BF38) // Updated
VARPTR(D2CLIENT, GoldDialogAction, DWORD, 0x102B38) // Updated
VARPTR(D2CLIENT, NoPickUp, DWORD, 0x11C114)

ASMPTR(D2CLIENT, GenerateSpell_I, 0x4F800) //6FAFF800 6FAFFF08  |. E8 F3F8FFFF    CALL D2Client.6FAFF800                   ; \D2Client.6FAFF800
ASMPTR(D2CLIENT, TestPvpFlag_I, 0x30DD0) // Updated
ASMPTR(D2CLIENT, LoadUIImage_I, 0xBEED0) // Updated
ASMPTR(D2CLIENT, LoadAct_1, 0x409E0) // Updated
ASMPTR(D2CLIENT, LoadAct_2, 0x406A0) // Updated
ASMPTR(D2CLIENT, GetUnitName_I, 0x1F3D0) // Updated
ASMPTR(D2CLIENT, InputCall_I, 0x79670) // Updated
ASMPTR(D2CLIENT, Interact_I, 0x5CDD0) // Updated
ASMPTR(D2CLIENT, clickParty_I, 0x78D10) // Updated
ASMPTR(D2CLIENT, clickParty_II, 0x62400)
ASMPTR(D2CLIENT, clickLocPtrs, 0xDCAE4) // Updated
ASMPTR(D2CLIENT, ClickShopItem_I, 0x191C0) // Updated
ASMPTR(D2CLIENT, ShopAction_I, 0x19E00) // Updated
ASMPTR(D2CLIENT, clickBelt_I, 0x81FB0) // Updated
ASMPTR(D2CLIENT, clickBeltRight_I, 0x83DF0) // Updated
ASMPTR(D2CLIENT, clickItemRight_I, 0xB60F0) // Updated
ASMPTR(D2CLIENT, GetItemDesc_I, 0x8A140) // Updated
ASMPTR(D2CLIENT, MercItemAction_I, 0x798E0) // Updated
ASMPTR(D2CLIENT, Attack_I, 0xDC6E4) // Updated
ASMPTR(D2CLIENT, AssignPlayer_I, 0x20D60)
ASMPTR(D2CLIENT, TakeWaypoint_I, 0x112F3)

//6FB37206   > B9 40FCBC6F    MOV ECX,D2Client.6FBCFC40
ASMPTR(D2CLIENT, Say_I, 0x87206)
ASMPTR(D2CLIENT, PlaySound_I, 0x31FA0) // Updated
ASMPTR(D2CLIENT, PerfomNpcAction_1,0x191C0) // Updated
ASMPTR(D2CLIENT, TakeWP_I, 0x797E0) // Updated & Actually it's the D2CLIENT_SendPacketWithTheSize9(DWORD dwArg1, DWORD dwArg2)
ASMPTR(D2CLIENT, clickMap_1, 0x102B7C) // Updated
ASMPTR(D2CLIENT, clickMap_2, 0x11C1D8) // Updated
ASMPTR(D2CLIENT, clickMap_3, 0x5D5A0) // Updated
ASMPTR(D2CLIENT, clickMap_4, 0x5CEF9) // Updated
ASMPTR(D2CLIENT, GetMinionCount_I, 0xC3340)

FUNCPTR(D2COMMON, AddRoomData, void __stdcall, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom), -10184) // Updated
FUNCPTR(D2COMMON, RemoveRoomData, void __stdcall, (Act * ptAct, int LevelId, int Xpos, int Ypos, Room1 * pRoom), -11009) // Updated
FUNCPTR(D2COMMON, GetLevelTxt, LevelTxt * __stdcall, (DWORD levelno), 0x73A10) // Updated
FUNCPTR(D2COMMON, GetObjectTxt, ObjectTxt * __stdcall, (DWORD objno), 0x37300) // Updated
FUNCPTR(D2COMMON, GetLayer, AutomapLayer2* __fastcall, (DWORD dwLevelNo), 0x73870) // Updated
FUNCPTR(D2COMMON, GetLevel, Level * __fastcall, (ActMisc *pMisc, DWORD dwLevelNo), -11020) // Updated
FUNCPTR(D2COMMON, InitLevel, void __stdcall, (Level *pLevel), -10721) // updated
FUNCPTR(D2COMMON, GetItemFromInventory, UnitAny *__stdcall, (Inventory* inv),-11151) // Updated
FUNCPTR(D2COMMON, GetNextItemFromInventory, UnitAny *__stdcall, (UnitAny *pItem), -10770)// Updated
FUNCPTR(D2COMMON, GetItemText, ItemTxt *__stdcall, (DWORD dwItemNo), 0x5C980) // Updated
FUNCPTR(D2COMMON, GetUnitStat, DWORD __stdcall, (UnitAny* pUnit, DWORD dwStat, DWORD dwStat2), 0x34E20) // Updated
FUNCPTR(D2COMMON, GetUnitX, int __stdcall, (Path *hPath), -10178) // Updated
FUNCPTR(D2COMMON, GetUnitY, int __stdcall, (Path *hPath), -10191) // Updated
FUNCPTR(D2COMMON, MapToAbsScreen, void __stdcall, (long *pX, long *pY), -10912)// Updated
FUNCPTR(D2COMMON, AbsScreenToMap, void __stdcall, (long *ptMouseX, long *ptMouseY), -10725)// Updated
FUNCPTR(D2COMMON, AddLevel, BOOL __stdcall, (Level* pLevel),0x56550) // Updated
FUNCPTR(D2COMMON, LoadAct, Act* __stdcall, (DWORD ActNumber, DWORD MapId, DWORD Unk, DWORD Unk_2, DWORD Unk_3, DWORD Unk_4, DWORD TownLevelId, DWORD Func_1, DWORD Func_2), 0x56780)
FUNCPTR(D2COMMON, GetRoomFromUnit,  Room1* __stdcall, (UnitAny * ptUnit), -10366) // Updated
FUNCPTR(D2COMMON, GetSkillLevel, INT __stdcall, (UnitAny* pUnit, Skill* pSkill, BOOL bTotal), -10904) // Updated
FUNCPTR(D2COMMON, CheckUnitCollision, DWORD __stdcall, (UnitAny* pUnitA, UnitAny* pUnitB, DWORD dwBitMask), -10233) // Updated
FUNCPTR(D2COMMON, GetBaseStat, DWORD __stdcall, (UnitAny *Unit, DWORD dwStat, DWORD dwUkn), -10494) // Updated
FUNCPTR(D2COMMON, GetItemLevelRequirement, DWORD __stdcall, (UnitAny* pItem, UnitAny* pPlayer), -10113)
FUNCPTR(D2COMMON, GetCursorItem, UnitAny * __stdcall, (Inventory * ptInventory), -10310) // Updated
FUNCPTR(D2COMMON, GetUnitState, int __stdcall, (UnitAny *pUnit, DWORD dwStateNo), -10295) // Updated
//FUNCPTR(D2COMMON, GetItemFlag, DWORD __stdcall, (UnitAny *Item, DWORD dwFlagMask, DWORD dwLineNo, char *szFileName), -10554) // Updated
FUNCPTR(D2COMMON, GetItemFlag, DWORD __fastcall, (UnitAny *Item, DWORD dwFlagMask, DWORD dwLineNo, char *szFileName), -10554) // Updated
FUNCPTR(D2COMMON, GetItemUnit, UnitAny *__stdcall, (DWORD dwId), -10852) // Updated
FUNCPTR(D2COMMON, GetStatList, StatList* __stdcall, (UnitAny* pUnit, DWORD dwUnk, DWORD dwMaxEntries ), -10611) // Updated
FUNCPTR(D2COMMON, CopyStatList, DWORD __stdcall, (StatList* pStatList, Stat* pStatArray, DWORD dwMaxEntries), -10373) // Updated 
FUNCPTR(D2COMMON, GetItemQuality, DWORD __stdcall, (UnitAny* pItem), -10276) // Updated
FUNCPTR(D2COMMON, GetItemNameString, void __stdcall, (UnitAny *pItem, char *szItemName, int nLen), 0x2C440) // ??? Updated
FUNCPTR(D2COMMON, GetItemMagicalMods, CHAR* __stdcall, (WORD wPrefixNum), -11065) // Updated
FUNCPTR(D2COMMON, CheckWaypoint, DWORD __stdcall, (DWORD WaypointTable, DWORD dwLevelId), -10875) // Updated
FUNCPTR(D2COMMON, GetItemPrice, DWORD __stdcall, (UnitAny* MyUnit, UnitAny* pItem, DWORD U1_,DWORD U2_,DWORD U3_,DWORD U4_), -10122) // Updated
FUNCPTR(D2COMMON, GetQuestFlag, INT __stdcall, (VOID* QuestInfo, DWORD dwAct, DWORD dwQuest), -10600) // Updated
FUNCPTR(D2COMMON, GenerateOverheadMsg, OverheadMsg* __stdcall, (DWORD dwUnk, CHAR* szMsg, DWORD dwTrigger), -10041) // Updated
FUNCPTR(D2COMMON, FixOverheadMsg, VOID __stdcall, (OverheadMsg* pMsg, DWORD dwUnk), -10315) // Updated
FUNCPTR(D2COMMON, CheckCollision, DWORD __stdcall, (Room1* pRoom, DWORD dwX, DWORD dwY, DWORD dwBitMask), -10493) // Updated
FUNCPTR(D2COMMON, GetUnitMaxHP, DWORD __stdcall, (UnitAny* pUnit), -10445) // Updated
FUNCPTR(D2COMMON, CheckUnitSlot, DWORD __stdcall, (Inventory* pInventory, DWORD dwSlot), -10099)

VARPTR(D2COMMON, sqptDataTable,DWORD, 0x9B500) // Updated
ASMPTR(D2COMMON, DisplayOverheadMsg_I, -10375) // Updated
ASMPTR(D2COMMON, checkCollision_I, 0x58370) // Updated

FUNCPTR(D2NET, SendPacket, void __stdcall, (size_t aLen, DWORD arg1, BYTE* aPacket), -10036) // Updated
FUNCPTR(D2NET, ReceivePacket, void __stdcall, (BYTE *aPacket, DWORD aLen), 0x6510) // Updated
FUNCPTR(D2NET, ReceivePacket_I, void __stdcall, (BYTE *aPacket, DWORD aLen), -10002) // Updated

FUNCPTR(D2GFX, DrawAutomapCell, void __stdcall, (CellContext *context, DWORD xpos, DWORD ypos, RECT *cliprect, DWORD bright), -10061)
FUNCPTR(D2GFX, DrawAutomapCell2, void __stdcall, (CellContext *context, DWORD xpos, DWORD ypos, DWORD bright2, DWORD bright, BYTE *coltab), -10024) 
FUNCPTR(D2GFX, DrawRectangle, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwTrans), 0x7D10) // Updated
FUNCPTR(D2GFX, DrawLine, void __stdcall, (int X1, int Y1, int X2, int Y2, DWORD dwColor, DWORD dwUnk), 0x7CA0) // Updated
FUNCPTR(D2GFX, GetScreenSize, DWORD __stdcall, (), -10043)  // Updated
VARPTR(D2LAUNCH, BnData, BnetData *, 0x25ACC); // Updated

FUNCPTR(D2MULTI, PrintChannelText, void __stdcall, (CHAR *szText, DWORD dwColor),  0xF8C0) // Updated - Okaygo found new offset
FUNCPTR(D2MULTI, DoChat, VOID __fastcall, (VOID), 0xC650) // Updated
FUNCPTR(D2MULTI, WhisperIntercept_Jump, VOID __fastcall, (VOID), 0xFC3C+8)
VARPTR(D2MULTI, ChatBoxMsg, CHAR*, 0x38F18) // Updated 

FUNCPTR(D2CMP, InitCellFile, void __stdcall, (void *cellfile, CellFile **outptr, char *srcfile, DWORD lineno, DWORD filever, char *filename), -10006)
FUNCPTR(D2CMP, DeleteCellFile, void __stdcall, (CellFile *cellfile), -10106) 

ASMPTR(D2LANG, Say_II, 0xA6E0)
FUNCPTR(D2LANG, GetLocaleText, wchar_t* __fastcall, (WORD nLocaleTxtNo), -10005)
FUNCPTR(D2WIN, DrawSprites, VOID __fastcall, (VOID), 0xD680)
FUNCPTR(D2WIN, SetControlText, void* __fastcall, (Control* box, wchar_t* txt), -10095) // Updated
FUNCPTR(D2WIN, GetTextWidthFileNo, DWORD __fastcall, (wchar_t *wStr, DWORD* dwWidth, DWORD* dwFileNo), -10096) // Updated
FUNCPTR(D2WIN, SetTextSize, DWORD __fastcall, (DWORD dwSize), -10010) // Updated 
FUNCPTR(D2WIN, DrawText, void __fastcall, (wchar_t *wStr, int xPos, int yPos, DWORD dwColor, DWORD dwUnk), -10001) // Updated
FUNCPTR(D2GFX, GetHwnd, HWND __stdcall, (), -10078) // Updated
VARPTR(D2WIN, FirstControl, Control *, 0x5C718) // Updated 
VARPTR(D2WIN, FocusedControl, Control*, 0x5C718+0x10) // Updated

//6F8ED848   E8 33FEFFFF      CALL D2Win.6F8ED680


#define _D2PTRS_END	p_D2WIN_FocusedControl

#undef FUNCPTR
#undef VARPTR
#undef ASMPTR

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