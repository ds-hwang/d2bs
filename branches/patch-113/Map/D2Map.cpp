#include <algorithm>
#include <assert.h>

#include "D2Map.h"

#include "D2Structs.h"
#include "D2Ptrs.h"
#include "Constants.h"
#include "D2Helpers.h"

namespace Mapping
{

MapList D2Map::cache = MapList();

D2Map* D2Map::GetMap(Level* level)
{
	if(cache.size() != 0)
		if(cache.count(level->dwLevelNo) > 0)
			return cache[level->dwLevelNo];
	D2Map* map = new D2Map(level);
	cache[level->dwLevelNo] = map;
	return map;
}

void D2Map::ClearCache(void)
{
	for(MapList::iterator it = cache.begin(); it != cache.end(); it++)
		delete it->second;
	cache.clear();
}

D2Map::D2Map(const Level* level)
{
	lock = new CRITICAL_SECTION;
	InitializeCriticalSection(lock);

	assert(act != NULL);
	assert(level != NULL);

	this->level = level;
	this->act = level->pMisc->pAct;

	// get the map size
	height = level->dwSizeX * 5;
	width  = level->dwSizeY * 5;
	posX   = level->dwPosX * 5;
	posY   = level->dwPosY * 5;
	endX   = posX + height;
	endY   = posY + width;
	

	bool added = false;
	Room2* room = level->pRoom2First;
	if(!level->pRoom2First->pRoom1)
	{
		AddRoomData(room);
		added = true;
	}

	Build();

	if(added)
		RemoveRoomData(room);

	posX   = (level->dwPosX == -1 ? 0 : level->dwPosX * 5);
	posY   = (level->dwPosY == -1 ? 0 : level->dwPosY * 5);
}

D2Map::~D2Map(void)
{
	delete mapPoints;
	DeleteCriticalSection(lock);
	delete lock;
	lock = NULL;
}

void D2Map::Build(void)
{
	EnterCriticalSection(lock);

	mapPoints = new Matrix<CollisionFlag>(height, width);

	RoomList addedRooms;
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	AddRoom(level->pRoom2First, addedRooms, player);

	FillGaps();
	ThickenWalls();

	LeaveCriticalSection(lock);
}

void D2Map::AddRoom(Room2* const room, RoomList& rooms, UnitAny* player)
{
	if(!room || room->pLevel->dwLevelNo != level->dwLevelNo)
		return;

	// ignore rooms we've seen before
	if(std::find(rooms.begin(), rooms.end(), room) != rooms.end())
		return;

	rooms.push_back(room);

	// add the room if necessary
	bool added = false;
	if(!room->pRoom1)
	{
		AddRoomData(room);
		added = true;
	}

	// add the collision data to the map
	if(room->pRoom1)
		AddCollisionMap(room->pRoom1->Coll);

	// now do the same to every room near this one
	Room2** pRooms = room->pRoom2Near;
	for(DWORD i = 0; i < room->dwRoomsNear; i++)
		AddRoom(pRooms[i], rooms, player);

	// and remove the room afterwards if we added it
	if(added)
		RemoveRoomData(room);
}
void D2Map::AddCollisionMap(const CollMap* const map)
{
	if(map == NULL)
		return;

	int	x = map->dwPosGameX - posX, y = map->dwPosGameY - posY,
		height = map->dwPosGameX + map->dwSizeGameX - posX, width = map->dwPosGameY + map->dwSizeGameY - posY;

	WORD* p = map->pMapStart;

	for(int i = y; i < width && p != map->pMapEnd; i++)
		for(int j = x; j < height && p != map->pMapEnd; j++, p++)
			SetCollisionData(j, i, *p);
}
void D2Map::SetCollisionData(int x, int y, WORD value)
{
	// sync this to ensure that we don't try to add to a dead map or something
	EnterCriticalSection(lock);

	assert(IsValidPoint(Point(x, y), false));
	mapPoints->SetPoint(x, y, (CollisionFlag)(value & DefaultFlags));

	LeaveCriticalSection(lock);
}
bool D2Map::IsRoomWalkable(Room2* const pRoom2) const
{
	bool bAdded = FALSE;
	CollMap* pCol = NULL;
		if(!pRoom2->pRoom1)
	{
		bAdded = TRUE;
		D2COMMON_AddRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
	}

	if(pRoom2->pRoom1)
		pCol = pRoom2->pRoom1->Coll;

	if(!pCol)
	{
		if(bAdded)
			D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
		return FALSE;
	}
	int x = pCol->dwPosGameX - pRoom2->pLevel->dwPosX * 5;
	int y = pCol->dwPosGameY - pRoom2->pLevel->dwPosY * 5;
	int nCx = pCol->dwSizeGameX;
	int nCy = pCol->dwSizeGameY;

	int nLimitX = x + nCx;
	int nLimitY = y + nCy;

	WORD* p = pCol->pMapStart;

	for(int j = y; j < nLimitY; j++)		
	{
		
		for (int i = x; i < nLimitX; i++)
		{
			if (*p %2 == 0){
			if(bAdded)
				D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
				return TRUE;
			}				
			p++;
		}

	}
	if(bAdded)
		D2COMMON_RemoveRoomData(D2CLIENT_GetPlayerUnit()->pAct, pRoom2->pLevel->dwLevelNo, pRoom2->dwPosX, pRoom2->dwPosY, D2CLIENT_GetPlayerUnit()->pPath->pRoom1);
		return FALSE;

}
bool D2Map::IsGap(int x, int y, bool abs) const
{
	const int gapSize = 3;
	Point pt(x, y);
	assert(IsValidPoint(pt, abs));
	if(SpaceIsWalkable(pt, abs))
		return false;

	int spaces = 0;

	for(int i = x-gapSize-1; i <= x+gapSize+1 && spaces < gapSize; i++)
	{
		Point pt(i, y);
		if(IsValidPoint(pt, abs))
			spaces = (SpaceIsWalkable(pt, abs) ? 0 : spaces+1);
	}

	if(spaces < gapSize)
		return true;

	for(int i = y-gapSize-1, spaces = 0; i <= y+gapSize+1 && spaces < gapSize; i++)
	{
		Point pt(x, i);
		if(IsValidPoint(pt, abs))
			spaces = (SpaceIsWalkable(pt, abs) ? 0 : spaces+1);
	}

	return spaces < gapSize;
}
void D2Map::FillGaps(void)
{
	EnterCriticalSection(lock);

	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(IsGap(i, j, false))
				SetCollisionData(i, j, 2);

	LeaveCriticalSection(lock);
}
void D2Map::ThickenWalls(void)
{
	const int ThickenAmount = 1;

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Point pt(i, j);
			if(SpaceIsWalkable(pt, false) || GetMapData(pt, false) == D2Map::ThickenedWall)
				continue;

			for(int x = i-ThickenAmount; x <= i+ThickenAmount; x++)
			{
				for(int y = j-ThickenAmount; y <= j+ThickenAmount; y++)
				{
					Point point(x, y);
					if(IsValidPoint(point, false) && SpaceIsWalkable(point, false))
						SetCollisionData(x, y, D2Map::ThickenedWall);
				}
			}
		}
	}

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Point pt(i, j);
			if(GetMapData(pt, false) == D2Map::ThickenedWall)
				SetCollisionData(i, j, D2Map::Wall | D2Map::BlockWalk | D2Map::BlockPlayer);
		}
	}
}

Point D2Map::AbsToRelative(const Point& point) const
{
	return Point(point.first - posX, point.second - posY);
}
Point D2Map::RelativeToAbs(const Point& point) const
{
	return Point(point.first + posX, point.second + posY);
}

int D2Map::GetMapData(const Point& point, bool abs) const
{
	if(abs) return GetMapData(AbsToRelative(point), false);

	assert(IsValidPoint(point, abs));

	EnterCriticalSection(lock);
	CollisionFlag value = mapPoints->GetPoint(point.first, point.second);
	LeaveCriticalSection(lock);

	return value;
}
bool D2Map::IsValidPoint(const Point& point, bool abs) const
{
	if(abs) return IsValidPoint(AbsToRelative(point), false);
	return !!(point.first >= 0 && point.second >= 0 && point.first < height && point.second < width);
}

void D2Map::GetExits(ExitArray& exits) const
{
	static const Point empty(0,0);
	EnterCriticalSection(lock);

	RoomList added;

	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		if(!room->pRoom1)
		{
			AddRoomData(room);
			added.push_back(room);
		}

		if(room->pLevel->dwLevelNo != level->dwLevelNo)
			continue;

		Point x1(room->dwPosX * 5, room->dwPosY * 5),
			  x2(room->dwPosX * 5, room->dwPosY * 5 + room->dwSizeY * 5),
			  x3(room->dwPosX * 5 + room->dwSizeX * 5, room->dwPosY * 5 + room->dwSizeY * 5),
			  x4(room->dwPosX * 5 + room->dwSizeX * 5, room->dwPosY * 5);

		Room2** rooms = room->pRoom2Near;
		for(DWORD i = 0; i < room->dwRoomsNear; i++)
		{
			if(!rooms[i]->pRoom1)
			{
				AddRoomData(rooms[i]);
				added.push_back(rooms[i]);
			}

			if(rooms[i]->pLevel->dwLevelNo != level->dwLevelNo && D2Map::IsRoomWalkable(rooms[i]))
			{
				// does this link already exist?
				bool exists = false;
				ExitArray::iterator begin = exits.begin(), last = exits.end();
				for(ExitArray::iterator it = begin; it != last; it++)
				{
					if(it->Target == rooms[i]->pLevel->dwLevelNo)
					{
						exists = true;
						break;
					}
				}
				if(exists)
					continue;

				// definitely a link, doesn't exist, is it walkable?
				// find the side most adjacent to this one
				Point y1(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5),
					  y2(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeY * 5),
					  y3(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeY * 5),
					  y4(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5);

				Point start(0,0), end(0,0);

				if(x1.first == posX && y3.first == posX) //Left Side
					{ start = x1; end = x2; }
				else if(x2.second== endY  && y4.second ==endY )
					{ start = x2; end = x3; } // bottom side
				else if(x3.first  == endX && y1.first  == endX)
					{ start = x3; end = x4; } // Right Side
				else if(x4.second == posY && y2.second == posY)
					{ start = x4; end = x1; }//top side

				if(start != empty && end != empty)
				{
					if(start.first == endX)  start.first--;
					if(start.second == endY) start.second--;
					if(end.first == endX)    end.first--;
					if(end.second == endY)   end.second--;

					int xstep = end.first - start.first,
						ystep = end.second - start.second;
					xstep = xstep > 0 ? 1 : (xstep < 0 ? -1 : 0);
					ystep = ystep > 0 ? 1 : (ystep < 0 ? -1 : 0);

					int spaces = 0;
					Point midpoint(start.first, start.second);
					bool found = false;
					for(int x = 0, y = 0; !found && midpoint != end; x += xstep, y += ystep)
					{
						midpoint = Point(start.first + x, start.second + y);
						if(SpaceIsWalkable(midpoint, true))					
						{
							spaces++;
							if(spaces == 2) // changed to 2 to find exit out of act 4 town
							{
								exits.push_back(Exit(midpoint, rooms[i]->pLevel->dwLevelNo, Linkage, 0));								
								found = true;								
							}
						}
						else
							spaces = 0;
					}
				}
			}
		}

		for(PresetUnit* preset = room->pPreset; preset; preset = preset->pPresetNext)
		{
			if(preset->dwType == UNIT_TILE)
			{
				DWORD levelId = GetTileLevelNo(room, preset->dwTxtFileNo);
				if(levelId != 0)
				{
					Point loc((room->dwPosX*5)+preset->dwPosX, (room->dwPosY*5)+preset->dwPosY);
					bool exists = false;

					ExitArray::iterator start = exits.begin(), end = exits.end();
					for(ExitArray::iterator it = start; it != end; it++)
					{
						if(it->Position == loc)
						{
							exists = true;
							break;
						}
					}

					if(!exists)
					{
						exits.push_back(Exit(loc, levelId, Tile, preset->dwTxtFileNo));
					}
				}
			}
		}
	}

	RoomList::iterator start = added.begin(), last = added.end();
	for(RoomList::iterator it = start; it != last; it++)
		RemoveRoomData(*it);

	LeaveCriticalSection(lock);
}

bool D2Map::SpaceHasFlag(int flag, const Point& point, bool abs) const
{
	return ((GetMapData(point, abs) & flag) == flag);
}
bool D2Map::PathHasFlag(int flag, const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceHasFlag(flag, points[i], abs))
			return false;
	return true;
}

bool D2Map::SpaceIsWalkable(const Point& point, bool abs) const
{	
	return !SpaceHasFlag(D2Map::Avoid, point, abs)		  &&
		   !(SpaceHasFlag(D2Map::BlockWalk, point, abs) || SpaceHasFlag(D2Map::BlockPlayer, point, abs)  ||
		     SpaceHasFlag(D2Map::ClosedDoor, point, abs) || SpaceHasFlag(D2Map::NPCCollision, point, abs) ||
		     SpaceHasFlag(D2Map::Object, point, abs));
}
bool D2Map::SpaceHasLineOfSight(const Point& point, bool abs) const
{
	return !SpaceHasFlag(D2Map::Avoid, point, abs) && !SpaceHasFlag(D2Map::BlockLineOfSight, point, abs);
}

bool D2Map::SpaceIsInvalid(const Point& point, bool abs) const { return SpaceHasFlag(D2Map::Avoid, point, abs); }

bool D2Map::PathIsWalkable(const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceIsWalkable(points[i], abs))
			return false;
	return true;
}
bool D2Map::PathHasLineOfSight(const PointList& points, bool abs) const
{
	int count = points.size();
	for(int i = 0; i < count; i++)
		if(!SpaceHasLineOfSight(points[i], abs))
			return false;
	return true;
}

void D2Map::Dump(const char* file, const PointList& points) const
{
	FILE* f = NULL;
	fopen_s(&f, file, "wt");
	if(!f) return;
	fprintf(f, "Map (%d x %d) at (%d, %d) for area %s (%d):\n", width, height, posX, posY, D2COMMON_GetLevelText(level->dwLevelNo)->szName, level->dwLevelNo);

//	fprintf(f, "Exits:\n");

//	ExitArray exits;
//	GetExits(exits);
//	int i = 1;
//	for(ExitArray::iterator it = exits.begin(); it != exits.end(); it++)
//	{
//		Point rel = AbsToRelative(it->Location);
//		fprintf(f, "Exit %d: at (%d, %d, relative: %d, %d) of type %s to area %d (tile id: %d)\n", i++,
//						it->Location.first, it->Location.second, rel.first, rel.second,
//						it->Type == Linkage ? "Area Linkage" : "Tile", it->TargetLevel, it->TileId);
//	}

//	fprintf(f, "\n");

	PointList::const_iterator begin = points.begin(), end = points.end();
	Point first = *begin, last = *(end-1);

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Point pt(i, j);
			char c = ' ';
			if(SpaceHasFlag(D2Map::Avoid, pt, false))
				c = 'A';
			else if(pt == first)
				c = 'S';
			else if(pt == last)
				c = 'E';
			else if(std::find(begin, end, pt) != end)
				c = 'P';
			else if(SpaceIsInvalid(pt, false))
				c = '?';
			else if(GetMapData(pt, false) == D2Map::ThickenedWall)
				c = 'T';
			else if(!SpaceIsWalkable(pt, false))
				c = 'X';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

}
