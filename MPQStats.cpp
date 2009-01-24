#include "D2BS.h"



MPQTables BaseStatTable[] = {
	{0x6FDF0438,0x6FDF0434, ItemTable, 0xC2, 0xFFFF},
	{0xA78, 0xA80, MonstatsTable, 0xF6, 0xFFFF},
	{0xB8C, 0xB94, SkilldescTable, 0x72, 0xFFFF},
	{0xB98, 0xBA0, SkillTable, 0xEF, 0xFFFF},
	{0x6FDF13E4,0x6FDF13E8, ObjetTable, 0x9E, 0xFFFF},
	{0xB64, 0xB6C, MissileTable, 0x93, 0xFFFF},
	{0xA90, 0xA98, Monstats2Table, 0x6E, 0xFFFF},
	{0xBCC, 0xBD4, ItemstatcostTable, 0x34, 0xFFFF},
	{0xC58, 0xC5C, LevelTable, 0x8E, 0xFFFF},
	{0xC60, 0xC5C, LeveldefTable, 0x2B, 0xFFFF},
	{0x6FDF1430, 0x6FDF1434, lvlmazeTable, 0x8, 0xFFFF}, // v
	{0x6FDF1438, 0x6FDF143C, lvlsubTable, 0x17, 0xFFFF}, // v
	{0x6FDF1428, 0x6FDF142C, lvlwarpTable, 0xD, 0xFFFF}, // v
	{0xC64, 0xC68, lvlprestTable, 0x18, 0xFFFF},
	{0x6FDF1418, 0x6FDF1420, lvltypesTable, 0x23, 0xFFFF}, // v
	{0xBC4, 0xBC8, CharstatsTable, 0x49, 0xFFFF},
	{0xC18, 0xC1C, SetitemTable, 0x60, 0xFFFF},
	{0xC24, 0xC28, UniqueItemTable, 0x46, 0xFFFF},
	{0xC0C, 0xC10, SetsTable, 0x44, 0xFFFF},
	{0xBF8, 0xBFC, ItemtypesTable, 0x24, 0xFFFF},
	{0x6FDF0478, 0x6FDF0474, RunesTable, 0x30, 0xFFFF}, // v
	{0x6FDF148C, 0x6FDF1490, CubemainTable, 0xC4, 0xFFFF}, //v
	{0x6FDF0470, 0x6FDF046C, GemsTable, 0x2A, 0xFFFF},
	{0x6FDF13D0, 0x0, ExpierenceTable, 0x9, 0xFFFF}, // v
	{0xBE8, 0xBF0, PettypeTable, 0x15, 0xFFFF},
	{0xAD4, 0xADC, SuperUnqiuesTable, 0x15, 0xFFFF},
};

/*
MPQTables BaseStatTable[] = {
	{ItemTable,0xC2,0xFFFF,0x6FDF0438,0x6FDF0434},
	{MonstatsTable,0xF6,0xFFFF,0xB8C,0xB94},
	{SkilldescTable,0x72,0xFFFF,0xB98,0xBA0},
	{SkillTable,0xEF,0xFFFF,0x6FDF13E4,0x6FDF13E8},
	{ObjetTable,0x9E,0xFFFF,0xB64,0xB6C},
	{MissileTable,0x93,0xFFFF,0xA90,0xA98},
	{Monstats2Table,0x6E,0xFFFF,0xBCC,0xBD4},
	{ItemstatcostTable,0x34,0xFFFF,0xC58,0xC5C},
	{LevelTable,0x8E,0xFFFF,0xC60,0xC5C},
	{LeveldefTable,0x2B,0xFFFF,0x6FDF1430,0x6FDF1434},
	{lvlmazeTable,0x8,0xFFFF,0x6FDF1438,0x6FDF143C},
	{lvlsubTable,0x17,0xFFFF,0x6FDF1428,0x6FDF142C},
	{lvlwarpTable,0xD,0xFFFF,0xC64,0xC68},
	{lvlprestTable,0x18,0xFFFF,0x6FDF1418,0x6FDF1424},
	{lvltypesTable,0x23,0xFFFF,0xBC4,0xBC8},
	{CharstatsTable,0x49,0xFFFF,0xC18,0xC1C},
	{SetitemTable,0x60,0xFFFF,0xC24,0xC28},
	{UniqueItemTable,0x46,0xFFFF,0xC0C,0xC10},
	{SetsTable,0x44,0xFFFF,0xBF8,0xBFC},
	{ItemtypesTable,0x24,0xFFFF,0x6FDF0478,0x6FDF0474},
	{RunesTable,0x30,0xFFFF,0x6FDF148C,0x6FDF148C+4},
	{CubemainTable,0xC4,0xFFFF,0x6FDF0470,0x6FDF0470-4},
	{GemsTable,0x2A,0xFFFF,0x6FDF13D0,0x0},
	{ExpierenceTable,0x9,0xFFFF,0xBE8,0xBF0},
	{PettypeTable,0x15,0xFFFF,0xAD4,0xADC},
	{SuperUnqiuesTable,0x15,0xFFFF,0x1,0x332F68},
};
*/

DWORD GetBaseTable(INT nBaseStat, INT nClassId)
{
	DWORD dwResult = NULL;
	DWORD dwD2MPQTable = NULL;
	DWORD dwMaxEntries;

	if(nBaseStat < sizeof(BaseStatTable))
	{
		DWORD dwTableOffset = BaseStatTable[nBaseStat].dwEntry;

		// Locate the table.
		if(dwTableOffset <= 0xFFFFFF)
			dwD2MPQTable = (*p_D2COMMON_sqptDataTable);
		else dwD2MPQTable = NULL;

		DWORD dwMaxEntriesOffset = BaseStatTable[nBaseStat].dwMaxEntriesOffset;

		if(dwMaxEntriesOffset)
			dwMaxEntries = *(DWORD*)(dwMaxEntriesOffset + dwD2MPQTable);
		else dwMaxEntries = 0xFF;

#pragma warning( disable : 18 )
		if(nClassId < dwMaxEntries)
#pragma warning( default : 18 )
		{
			DWORD dwMultiplicator = BaseStatTable[nBaseStat].pTable[BaseStatTable[nBaseStat].wTableSize-1].wPtrToAdd;
			DWORD dwTable = nClassId * dwMultiplicator;
			dwResult = *(DWORD*)(dwTableOffset + dwD2MPQTable) + dwTable;
		}
	}

	return dwResult;
}


DWORD FillBaseStat(JSContext* cx, jsval *argv, INT nBaseStat, INT nClassId, INT nStatNumber)
{
  MPQTable* pTable = BaseStatTable[nBaseStat].pTable;
  DWORD dwRetValue = GetBaseTable(nBaseStat, nClassId);
  return sub_6B00F455(cx, argv, dwRetValue, (INT)pTable, nStatNumber, BaseStatTable[nBaseStat].wTableSize);
}

DWORD sub_6B00F455(JSContext* cx, jsval *argv, int pD2Table, int pMPQTable, int nStatNumber, int wTableSize)
{
  int v5; // eargv@3
  int v6; // edi@3
  size_t v7; // esi@3
  char v8; // al@6
  int v9; // edx@11
  unsigned int v10; // esi@11
  unsigned int v11; // eax@12
  unsigned __int8 v12 = 0; // al@14
  char result = 0; // al@19
#pragma warning(disable: 4101)
  __int64 v14; // ST1C_8@19
  void *v16; // eax@24
  __int64 v17; // [sp+14h] [bp-8h]@19
#pragma warning(default: 4101)

  if ( !pD2Table || nStatNumber >= (int)(wTableSize - 1) )
    return 0;

  v5 = *(WORD *)(pMPQTable + 4 * nStatNumber);
  v6 = pMPQTable + 4 * nStatNumber;
  v7 = *(WORD *)(pMPQTable + 4 * nStatNumber + 4) - v5;
  if ( !v7 )
  {
    if ( *(BYTE *)(v6 + 2) == 26 )
    {
      v7 = 4;
      goto LABEL_6;
    }
    return 0;
  }
LABEL_6:
  v8 = *(BYTE *)(v6 + 2);
  if ( v8 == 1 || v8 == 9 || v8 == 10 || v8 == 16 )
  {
    v16 = malloc(v7 + 1);
    wTableSize = (int)v16;
    memset(v16, 0, v7 + 1);
    memcpy((void *)wTableSize, (const void *)(pD2Table + *(WORD *)v6), v7);
	*argv = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (CHAR*)wTableSize));
  }
  else
  {
    if ( v8 == 26 )
    {
      v10 = *(BYTE *)(v6 + 3);
      v9 = 0;
      if ( v10 > 0x1F )
      {
        v11 = v10 >> 5;
        v9 = 4 * (v10 >> 5);
        do
        {
          v10 -= 32;
          --v11;
        }
        while ( v11 );
      }
	  DWORD dwBuffer;
      memcpy(&dwBuffer, (const void *)(pD2Table + v9 + v5), 4u);
	  *argv = INT_TO_JSVAL(dwBuffer);
	 // Print("->> %d", (pD2Table + v9 + v5));
    //  v12 = 2 * ((argv & (1 << (char)v10)) != 0);
      return v12 | 1;
    }
    if ( pMPQTable != (DWORD)ExpierenceTable || v8 != 2 )
    {
      nStatNumber = 0;
      if ( v7 >= 5 )
        v7 = 4;
      memcpy(&nStatNumber, (const void *)(pD2Table + v5), v7);
      v12 = 2 * (BYTE)nStatNumber;
	*argv = INT_TO_JSVAL(nStatNumber);
      return v12 | 1;
    }
    wTableSize = 0;
    if ( v7 >= 5 )
      v7 = 4;
    memcpy(&wTableSize, (const void *)(pD2Table + v5), v7);
//	Print("%s", wTableSize);
//    *((DWORD *)&v17 + 1) = 0;
//    *(DWORD *)&v17 = wTableSize;
	JS_NewNumberValue(cx, wTableSize, argv);
  }
  return result;
}