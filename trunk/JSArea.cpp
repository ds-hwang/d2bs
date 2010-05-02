#include "JSArea.h"
#include "JSExit.h"
#include "CriticalSections.h"
#include "Area.h"
#include "CollisionMap.h"

#include "D2Ptrs.h"

EMPTY_CTOR(area)

void area_finalize(JSContext *cx, JSObject *obj)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);

	if(pArea)
	{
		if(pArea->ExitArray)
			JS_RemoveRoot(&pArea->ExitArray);
		JS_SetPrivate(cx, obj, NULL);
		delete pArea;
	}
}

JSAPI_PROP(area_getProperty)
{
	myArea* pArea = (myArea*)JS_GetPrivate(cx, obj);;
	if(!pArea)
		THROW_ERROR(cx, "Could not convert myArea object")

	Level* pLevel = GetLevel(pArea->AreaId);
	if(!pLevel)
		return JS_TRUE;

	switch(JSVAL_TO_INT(id))
		{
			case AUNIT_EXITS:
			{
				if(pArea->ExitArray == NULL)
				{
					pArea->ExitArray = JS_NewArrayObject(cx, 0, NULL);
					JS_AddRoot(&pArea->ExitArray);

					CriticalRoom cRoom;
					cRoom.EnterSection();

					CCollisionMap cMap;

					if(!cMap.CreateMap(pArea->AreaId))
					{
						*vp = JSVAL_FALSE;
						return JS_TRUE;
					}

					// TODO: clean this out too
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
							THROW_ERROR(cx, "Failed to create exit object!");
						}

						jsval a = OBJECT_TO_JSVAL(jsUnit);
						JS_SetElement(cx, (JSObject*)pArea->ExitArray, i, &a);
					}
				}
				*vp = OBJECT_TO_JSVAL((JSObject*)pArea->ExitArray);
			}
			break;
		case AUNIT_NAME:
			{
				char* name = GetAreaName(pArea);
				if(name)
					*vp = STRING_TO_JSVAL(JS_InternString((cx, name)));
			}
			break;
		// TODO: turn these into inline function calls or something
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

JSAPI_FUNC(my_getArea)
{
	if(!WaitForClientState())
		THROW_ERROR(cx, "Game not ready");

	int32 nArea = -1;
	if(argc == 1)
	{
		if(JSVAL_IS_INT(argv[0]))
			JS_ValueToECMAInt32(cx, argv[0], &nArea);
		else
			THROW_ERROR(cx, "Invalid parameter passed to getArea!");

		if(nArea < 0)
			THROW_ERROR(cx, "Invalid parameter passed to getArea!");
	}

	myArea* pArea = GetArea(nArea);
	JSObject* unit = BuildObject(cx, &area_class, NULL, area_props, pArea);
	if(!unit)
	{
		delete pArea;
		THROW_ERROR(cx, "Failed to build the Area object");
	}
	*rval = OBJECT_TO_JSVAL(unit);

	return JS_TRUE;
}
