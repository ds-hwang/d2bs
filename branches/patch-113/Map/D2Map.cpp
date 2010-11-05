#define _SECURE_SCL 0
#define _SECURE_SCL_THROWS 0
#define _HAS_ITERATOR_DEBUGGING 0

#include <algorithm>
#include <assert.h>

#include "D2BS.h"

#include "D2Map.h"

#include "D2Structs.h"
#include "D2Ptrs.h"
#include "Constants.h"
#include "D2Helpers.h"

namespace Mapping
{

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

	std::vector<std::pair<Point, Point> > potentials;

	for(int i = 0; i < height; i++)
	{
		Point start(i, 0);
		if(SpaceIsWalkable(start, false))
		{
			Point end(i+1, 0);
			for(; end.first < height; end.first++)
			{
				if(!SpaceIsWalkable(end, false))
				{
					end.first--;
					break;
				}
			}

			potentials.push_back(make_pair(start, end));
			break;
		}
	}

	for(int i = 0; i < height; i++)
	{
		Point start(i, width-1);
		if(SpaceIsWalkable(start, false))
		{
			Point end(i+1, width-1);
			for(; end.first < height; end.first++)
			{
				if(!SpaceIsWalkable(end, false))
				{
					end.first--;
					break;
				}
			}

			potentials.push_back(make_pair(start, end));
			break;
		}
	}

	for(int i = 0; i < width; i++)
	{
		Point start(0, i);
		if(SpaceIsWalkable(start, false))
		{
			Point end(0, i+1);
			for(; end.second < width; end.second++)
			{
				if(!SpaceIsWalkable(end, false))
				{
					end.second--;
					break;
				}
			}

			potentials.push_back(make_pair(start, end));
			break;
		}
	}

	for(int i = 0; i < width; i++)
	{
		Point start(height-1, i);
		if(SpaceIsWalkable(start, false))
		{
			Point end(height-1, i+1);
			for(end.second++; end.second < width; end.second++)
			{
				if(!SpaceIsWalkable(end, false))
				{
					end.second--;
					break;
				}
			}

			potentials.push_back(make_pair(start, end));
			break;
		}
	}

	PointList centers;
	for(std::vector<std::pair<Point, Point> >::iterator it = potentials.begin(); it != potentials.end(); it++)
	{
		Point start = it->first, end = it->second;
		int xdiff = end.first - start.first, ydiff = end.second - start.second;
		int xcenter = 0, ycenter = 0;

		if(xdiff > 0)
		{
			if(xdiff % 2) xcenter = start.first + ((xdiff - (xdiff % 2)) / 2);
			else xcenter = start.first + (xdiff / 2);
		}

		if(ydiff > 0)
		{
			if(ydiff % 2) ycenter = start.second + ((ydiff - (ydiff % 2)) / 2);
			else ycenter = start.second + (ydiff / 2);
		}

		Point center(xcenter ? xcenter : start.first, ycenter ? ycenter : start.second);
		center = RelativeToAbs(center);
		centers.push_back(center);
	}

	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next)
	{
		Room2** pRooms = room->pRoom2Near;
		for(DWORD i = 0; i < room->dwRoomsNear; i++)
		{
			if(pRooms[i]->pLevel->dwLevelNo != level->dwLevelNo)
			{
				int roomx = pRooms[i]->dwPosX * 5, roomy = pRooms[i]->dwPosY * 5;
				int roomh = roomx + (pRooms[i]->dwSizeX * 5), roomw = roomy + (pRooms[i]->dwSizeY * 5);

				for(PointList::iterator it = centers.begin(); it != centers.end(); it++)
				{
					Point p = *it;
					if(p.first >= roomx && p.first <= roomh && p.second >= roomy && p.second <= roomw)
					{
						Exit exit(p, pRooms[i]->pLevel->dwLevelNo, Linkage, 0);
						exits.push_back(exit);
					}
				}

				break;
			}
		}

		bool added = false;

		if(!room->pRoom1)
		{
			AddRoomData(room);
			added = true;
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

		if(added)
			RemoveRoomData(room);
	}

	char path[520];
	sprintf_s(path, 520, "%smap.txt", Vars.szPath);
	PointList pl;
	for(PointList::iterator it = centers.begin(); it != centers.end(); it++)
		pl.push_back(AbsToRelative(*it));
	Dump(path, pl);

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
