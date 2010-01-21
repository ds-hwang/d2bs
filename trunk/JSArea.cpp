#include "JSArea.h"
#include "D2Ptrs.h"
#include "D2Helpers.h"
#include "JSExits.h"
#include "CriticalSections.h"

EMPTY_CTOR(area)

void area_finalize(JSContext *cx, JSObject *obj)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(pArea)
	{
		JS_SetPrivate(cx, obj, NULL);
		delete pArea;
	}
}

JSAPI_PROP(area_getProperty)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(!pArea)
		return JS_FALSE;

	Level* pLevel = GetLevel(pArea->AreaId);
	if(!pLevel)
		return JS_FALSE;

	switch(JSVAL_TO_INT(id))
		{
			case AUNIT_EXITS:
			{
				if(pArea->ExitArray == NULL)
				{
					pArea->ExitArray = JS_NewArrayObject(cx, 0, NULL);

					CriticalRoom cRoom;
					cRoom.EnterSection();

					CCollisionMap cMap;

					if(!cMap.CreateMap(pArea->AreaId))
					{
						*vp = JSVAL_FALSE;
						return JS_TRUE;
					}

					LevelExit* ExitArray[255];
					int count = cMap.GetLevelExits(ExitArray);

					for(int i = 0; i < count; i++)
					{
						myExit* pExit = new myExit;
						pExit->id		= ExitArray[i]->dwTargetLevel;
						pExit->x		= ExitArray[i]->ptPos.x;
						pExit->y		= ExitArray[i]->ptPos.y;
						pExit->type		= ExitArray[i]->dwType;
						pExit->tileid	= ExitArray[i]->dwId;
						pExit->level	= pArea->AreaId;

						JSObject* jsUnit = BuildObject(cx, &exit_class, NULL, exit_props, pExit);
						if(!jsUnit)
						{
							delete pExit;
							pExit = NULL;
							THROW_ERROR(cx, obj, "Failed to create exit object!");
						}

						jsval a = OBJECT_TO_JSVAL(jsUnit);
						JS_SetElement(cx, pArea->ExitArray, i, &a);
					}
				}
				*vp = OBJECT_TO_JSVAL(pArea->ExitArray);
			}
			break;
		case AUNIT_NAME:
			{
				LevelTxt* pTxt = D2COMMON_GetLevelTxt(pArea->AreaId);
				if(pTxt)
					*vp = STRING_TO_JSVAL(JS_InternString(cx, pTxt->szName));
			}
			break;
		case AUNIT_X:
			*vp = INT_TO_JSVAL(pLevel->dwPosX);
			break;
		case AUNIT_Y:
			*vp = INT_TO_JSVAL(pLevel->dwPosY);
			break;
		case AUNIT_XSIZE:
			*vp = INT_TO_JSVAL(pLevel->dwSizeX);
			break;
		case AUNIT_YSIZE:
			*vp = INT_TO_JSVAL(pLevel->dwSizeY);
			break;
		case AUNIT_ID:
			*vp = INT_TO_JSVAL(pLevel->dwLevelNo);
			break;
	}

	return JS_TRUE;
}
