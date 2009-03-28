#include "MPQStats.h"
#include "D2Ptrs.h"
#include "Core.h"

#include "debugnew/debug_new.h"

MPQTables BaseStatTable[] = {
	//	DWORD dwEntry, DWORD dwMaxEntriesOffset, BinField* pTable, WORD wTableSize, WORD wUnknown
	{0x6FDF0438,	0x6FDF0434,	itemTable,			ARRAYSIZE(itemTable),			0xFFFF},
	{0xA78,			0xA80,		monstatsTable,		ARRAYSIZE(monstatsTable),		0xFFFF},
	{0xB8C,			0xB94,		skilldescTable,		ARRAYSIZE(skilldescTable),		0xFFFF},
	{0xB98,			0xBA0,		skillsTable,		ARRAYSIZE(skillsTable),			0xFFFF},
	{0x6FDF13E4,	0x6FDF13E8,	objectsTable,		ARRAYSIZE(objectsTable),		0xFFFF},
	{0xB64,			0xB6C,		missilesTable,		ARRAYSIZE(missilesTable),		0xFFFF},
	{0xA90,			0xA98,		monstats2Table,		ARRAYSIZE(monstats2Table),		0xFFFF},
	{0xBCC,			0xBD4,		itemstatcostTable,	ARRAYSIZE(itemstatcostTable),	0xFFFF},
	{0xC58,			0xC5C,		levelsTable,		ARRAYSIZE(levelsTable),			0xFFFF},
	{0xC60,			0x00,		leveldefsTable,		ARRAYSIZE(leveldefsTable),		0xFFFF},
	{0x6FDF1430,	0x6FDF1434,	lvlmazeTable,		ARRAYSIZE(lvlmazeTable),		0xFFFF},
	{0x6FDF1438,	0x6FDF143C,	lvlsubTable,		ARRAYSIZE(lvlsubTable),			0xFFFF}, // v
	{0x6FDF1428,	0x6FDF142C,	lvlwarpTable,		ARRAYSIZE(lvlwarpTable),		0xFFFF}, // v
	{0xC64,			0xC68,		lvlprestTable,		ARRAYSIZE(lvlprestTable),		0xFFFF},
	{0x6FDF1418,	0x6FDF1424,	lvltypesTable,		ARRAYSIZE(lvltypesTable),		0xFFFF}, // v - Fixed to fully dump the same as d2jsp - TechnoHunter
	{0xBC4,			0xBC8,		charstatsTable,		ARRAYSIZE(charstatsTable),		0xFFFF},
	{0xC18,			0xC1C,		setitemsTable,		ARRAYSIZE(setitemsTable),		0xFFFF},
	{0xC24,			0xC28,		uniqueitemsTable,	ARRAYSIZE(uniqueitemsTable),	0xFFFF},
	{0xC0C,			0xC10,		setsTable,			ARRAYSIZE(setsTable),			0xFFFF},
	{0xBF8,			0xBFC,		itemtypesTable,		ARRAYSIZE(itemtypesTable),		0xFFFF},
	{0x6FDF0478,	0x6FDF0474,	runesTable,			ARRAYSIZE(runesTable),			0xFFFF}, // v
	{0x6FDF148C,	0x6FDF1490,	cubemainTable,		ARRAYSIZE(cubemainTable),		0xFFFF}, //v
	{0x6FDF0470,	0x6FDF046C,	gemsTable,			ARRAYSIZE(gemsTable),			0xFFFF},
	{0x6FDF13D0,	0x0,		experienceTable,	ARRAYSIZE(experienceTable),		0xFFFF}, // v - doesnt tap the last 2 levels of exp, ends at level 97 - TechnoHunter
	{0xBE8,			0xBF0,		pettypeTable,		ARRAYSIZE(pettypeTable),		0xFFFF},
	{0xAD4,			0xADC,		superuniquesTable,	ARRAYSIZE(superuniquesTable),	0xFFFF},
};

DWORD GetBaseTable(INT nBaseStat, INT nClassId)
{
	DWORD dwResult = NULL;
	DWORD dwD2MPQTable = NULL;
	DWORD dwMaxEntries;

	if(nBaseStat < sizeof(BaseStatTable))
	{
		DWORD dwTableOffset = BaseStatTable[nBaseStat].dwEntry;

		// Locate the table.
		if(dwTableOffset <= 0xFFFF)
			dwD2MPQTable = (*p_D2COMMON_sqptDataTable);
		else dwD2MPQTable = NULL;

		DWORD dwMaxEntriesOffset = BaseStatTable[nBaseStat].dwMaxEntriesOffset;

		if(dwMaxEntriesOffset)
			dwMaxEntries = *(DWORD*)(dwMaxEntriesOffset + dwD2MPQTable);
		else dwMaxEntries = 0xFF;

		if((DWORD)nClassId < dwMaxEntries)
		{
			DWORD dwMultiplicator = BaseStatTable[nBaseStat].pTable[BaseStatTable[nBaseStat].wTableSize-1].dwFieldOffset;
			DWORD dwTable = nClassId * dwMultiplicator;
			dwResult = *(DWORD*)(dwTableOffset + dwD2MPQTable) + dwTable;
		}
	}

	return dwResult;
}


DWORD FillBaseStat(JSContext* cx, jsval *argv, INT nBaseStat, INT nClassId, INT nStatNumber, CHAR* szStat)
{
	BinField* pTable = BaseStatTable[nBaseStat].pTable;
	DWORD dwRetValue = GetBaseTable(nBaseStat, nClassId);

	if(szStat)
	{
		nStatNumber = -1;

		for(INT i = 0; i < BaseStatTable[nBaseStat].wTableSize; i++)
		{
			if(!_strcmpi(szStat, pTable[i].szFieldName))
				nStatNumber = i;
		}

		if(nStatNumber == -1)
			return FALSE;
	}

	if(dwRetValue)
	{
		if(nStatNumber > BaseStatTable[nBaseStat].wTableSize)
			return FALSE;

		DWORD dwBuffer = 0;
		WORD wBuffer = 0;
		BYTE bBuffer = 0;
		CHAR* szBuffer = NULL;
		
		DWORD dwHelperSize = pTable[nStatNumber + 1].dwFieldOffset - pTable[nStatNumber].dwFieldOffset;

		if(dwHelperSize > 4)
			dwHelperSize = 4;
		
		switch(pTable[nStatNumber].eFieldType)
		{
			case FIELDTYPE_DATA_ASCII:
				szBuffer = (CHAR*)malloc(pTable[nStatNumber].dwFieldLength + 1);
				memset(szBuffer, NULL, pTable[nStatNumber].dwFieldLength + 1);
				memcpy_s(szBuffer, pTable[nStatNumber].dwFieldLength + 1, (BYTE*)(dwRetValue+pTable[nStatNumber].dwFieldOffset), pTable[nStatNumber].dwFieldLength + 1);
				(*argv) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szBuffer));
				free(szBuffer);
				return TRUE;

			case FIELDTYPE_DATA_DWORD:
				memcpy(&dwBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(DWORD));
				JS_NewNumberValue(cx,(jsdouble)dwBuffer,argv);
				return TRUE;

			case FIELDTYPE_CALC_TO_DWORD:
			case FIELDTYPE_NAME_TO_DWORD:
			case FIELDTYPE_DATA_DWORD_2:
				memcpy(&dwBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(DWORD));
				(*argv) = INT_TO_JSVAL(dwBuffer);
				return TRUE;
			
			case FIELDTYPE_UNKNOWN_11:
				memcpy(&dwBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(DWORD));
				(*argv) = INT_TO_JSVAL(dwBuffer);
				return TRUE;

			case FIELDTYPE_NAME_TO_INDEX_2:
			case FIELDTYPE_NAME_TO_WORD_2:
				memcpy(&wBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(WORD));
				if (wBuffer >= 0xFFFF){
					wBuffer = ((wBuffer - 0xFFFF) * -1);
				}
				(*argv) = INT_TO_JSVAL(wBuffer);
				return TRUE;

			case FIELDTYPE_NAME_TO_INDEX:
			case FIELDTYPE_NAME_TO_WORD:
			case FIELDTYPE_KEY_TO_WORD:
			case FIELDTYPE_DATA_WORD:
			case FIELDTYPE_CODE_TO_WORD:
				memcpy(&wBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(WORD));
				(*argv) = INT_TO_JSVAL(wBuffer);
				return TRUE;

			case FIELDTYPE_CODE_TO_BYTE:
			case FIELDTYPE_DATA_BYTE_2:
			case FIELDTYPE_DATA_BYTE:
				dwBuffer = NULL;
				memcpy(&dwBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), dwHelperSize);
				(*argv) = INT_TO_JSVAL(dwBuffer);
				return TRUE;

			case FIELDTYPE_DATA_BIT:
				memcpy(&dwBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(DWORD));
				(*argv) = INT_TO_JSVAL((dwBuffer & (1 << pTable[nStatNumber].dwFieldLength)) ? 1 : 0);
				return TRUE;

			case FIELDTYPE_ASCII_TO_CODE:
			case FIELDTYPE_DATA_RAW:
				szBuffer = (CHAR*)malloc(5);
				memset(szBuffer, NULL, 5);
				memcpy(szBuffer, (LPVOID)(dwRetValue+pTable[nStatNumber].dwFieldOffset), sizeof(DWORD));
				(*argv) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, szBuffer));
				free(szBuffer);
				return TRUE;

			case FIELDTYPE_MONSTER_COMPS:
				// ..? :E
				return FALSE;

		}
	}

	return FALSE;
}
