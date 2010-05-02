#include "Area.h"
#include "D2Helpers.h"
#include "D2Ptrs.h"

myArea* GetArea(__int32 area)
{
	area = (area == -1 ? GetPlayerArea() : area);

	Level* pLevel = GetLevel(area);

	if(!pLevel)
	{
		return NULL;
	}

	myArea* pArea = new myArea;
	if(!pArea)
	{
		return NULL;
	}

	pArea->AreaId = area;
	pArea->ExitArray = NULL;

	return pArea;
}

const char* GetAreaName(myArea* area)
{
	LevelText* txt = D2COMMON_GetLevelText(area->AreaId);
	if(txt && txt->szName)
		return txt->szName;
	return NULL;
}
