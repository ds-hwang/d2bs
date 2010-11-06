#define _SECURE_SCL 0
#define _SECURE_SCL_THROWS 0
#define _HAS_ITERATOR_DEBUGGING 0

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
	width  = level->dwSizeX * 5;
	height = level->dwSizeY * 5;
	posX   = level->dwPosX * 5;
	posY   = level->dwPosY * 5;

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

	// calling this invalidates the relative coordinates--we don't want that
	//ShrinkMap();
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
		dx = map->dwPosGameX + map->dwSizeGameX - posX, dy = map->dwPosGameY + map->dwSizeGameY - posY;

	WORD* p = map->pMapStart;

	for(int i = y; i < dy && p != map->pMapEnd; i++)
		for(int j = x; j < dx && p != map->pMapEnd; j++, p++)
			SetCollisionData(i, j, *p);
}
void D2Map::SetCollisionData(int x, int y, WORD value)
{
	// sync this to ensure that we don't try to add to a dead map or something
	EnterCriticalSection(lock);

	assert(IsValidPoint(Point(x, y), false));
	mapPoints->SetPoint(x, y, (CollisionFlag)value);

	LeaveCriticalSection(lock);
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
void D2Map::ShrinkMap(void)
{
	EnterCriticalSection(lock);

	bool blankRow = true;
	// walk over the rows, finding all blank (fully unwalkable) ones
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
			if(SpaceIsWalkable(Point(i, j), false))
				blankRow = false;

		if(blankRow)
		{
			// remove that row from the map, since it's irrelevant
			mapPoints->RemoveRow(i);
		}
	}

	bool blankCol = true;
	// walk over the columns, finding all blank (fully unwalkable) ones
	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
			if(SpaceIsWalkable(Point(j, i), false))
				blankCol = false;

		if(blankCol)
		{
			// remove that column from the map, since it's irrelevant
			mapPoints->RemoveColumn(i);
		}
	}

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
	EnterCriticalSection(lock);

	RoomList potentials, added;

	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		if(!room->pRoom1)
		{
			AddRoomData(room);
			added.push_back(room);
		}

		Room2** pRooms = room->pRoom2Near;
		for(DWORD i = 0; i < room->dwRoomsNear; i++)
		{
			if(!pRooms[i]->pRoom1)
			{
				AddRoomData(pRooms[i]);
				added.push_back(pRooms[i]);
			}

			if(pRooms[i]->pLevel->dwLevelNo != level->dwLevelNo)
			{
				potentials.push_back(pRooms[i]);
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

					for(ExitArray::iterator it = exits.begin(); it != exits.end(); it++)
					{
						if(it->Location.first == loc.first && it->Location.second == loc.second)
						{
							exists = true;
							break;
						}
					}

					if(!exists)
					{
						Exit exit(loc, levelId, Tile, preset->dwTxtFileNo);
						exits.push_back(exit);
					}
				}
			}
		}
	}

	RoomList scanned;
	RoomList::iterator begin = potentials.begin(), end = potentials.end();
	for(RoomList::iterator it = begin; it != end; it++)
	{
		Point x1((*it)->dwPosX * 5, (*it)->dwPosY * 5),
			  x2((*it)->dwPosX * 5, (*it)->dwPosY * 5 + (*it)->dwSizeY * 5),
			  x3((*it)->dwPosX * 5 + (*it)->dwSizeX * 5, (*it)->dwPosY * 5 + (*it)->dwSizeY * 5),
			  x4((*it)->dwPosX * 5 + (*it)->dwSizeX * 5, (*it)->dwPosY * 5);

		Room2** rooms = (*it)->pRoom2Near;
		for(DWORD i = 0; i < (*it)->dwRoomsNear; i++)
		{
			if(!rooms[i]->pRoom1)
			{
				AddRoomData(rooms[i]);
				added.push_back(rooms[i]);
			}

			if(std::find(scanned.begin(), scanned.end(), rooms[i]) != scanned.end())
				continue;

			scanned.push_back(rooms[i]);

			if(rooms[i]->pLevel->dwLevelNo == level->dwLevelNo)
			{
				// definitely a link, is it walkable?
				// find the side most adjacent to this one
				Point y1(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5),
					  y2(rooms[i]->dwPosX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeX * 5),
					  y3(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5 + rooms[i]->dwSizeY * 5),
					  y4(rooms[i]->dwPosX * 5 + rooms[i]->dwSizeX * 5, rooms[i]->dwPosY * 5);

				int s1 = x1.first-y1.first, s2 = x1.second-y1.second,
					s3 = x2.first-y2.first, s4 = x2.second-y2.second,
					s5 = x3.first-y3.first, s6 = x3.second-y3.second,
					s7 = x4.first-y4.first, s8 = x4.second-y4.second;

				Point midpoint(0,0);

				if((s1 == 0 && s2 == 0) && (s3 == 0 && s4 == 0))
					midpoint = Point((y1.first - y1.first) / 2, (y1.second - y1.second) / 2);
				else if((s3 == 0 && s4 == 0) && (s5 == 0 && s6 == 0))
					midpoint = Point((y2.first - y2.first) / 2, (y2.second - y2.second) / 2);
				else if((s5 == 0 && s6 == 0) && (s7 == 0 && s8 == 0))
					midpoint = Point((y3.first - y3.first) / 2, (y3.second - y3.second) / 2);
				else if((s7 == 0 && s8 == 0) && (s1 == 0 && s2 == 0))
					midpoint = Point((y4.first - y4.first) / 2, (y4.second - y4.second) / 2);

				if(midpoint.first != 0 && midpoint.second != 0)
				{
					if(SpaceIsWalkable(midpoint, true))
					{
						Exit exit(midpoint, (*it)->pLevel->dwLevelNo, Linkage, 0);
						exits.push_back(exit);
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
	return !SpaceHasFlag(D2Map::Avoid, point, abs) && !(SpaceHasFlag(D2Map::BlockWalk, point, abs) ||
		SpaceHasFlag(D2Map::BlockPlayer, point, abs) || SpaceHasFlag(D2Map::ClosedDoor, point, abs) ||
		SpaceHasFlag(D2Map::NPCCollision, point, abs) || SpaceHasFlag(D2Map::Object, point, abs));
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
	fprintf(f, "Map (%d x %d) at (%d, %d) for area %d:\n", width, height, posX, posY, level->dwLevelNo);

	fprintf(f, "Exits:\n");

	ExitArray exits;
	GetExits(exits);
	int i = 1;
	for(ExitArray::iterator it = exits.begin(); it != exits.end(); it++)
		fprintf(f, "Exit %d: at (%d, %d) of type %s to area %d (tile id: %d)\n", i++, it->Location.first, it->Location.second,
						it->Type == Linkage ? "Area Linkage" : "Tile", it->TargetLevel, it->TileId);

	fprintf(f, "\n");

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
