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
	Point pt(x, y);
	assert(IsValidPoint(pt, abs));
	if(GetMapData(pt, abs) % 2)
		return false;

	int spaces = 0;

	for(int i = x-2; i <= x+2 && spaces < 3; i++)
		spaces = ((i < 0 || i >= height || GetMapData(Point(i, y), abs) % 2) ? 0 : spaces+1);

	if(spaces < 3)
		return true;

	for(int i = y-2, spaces = 0; i <= y+2 && spaces < 3; i++)
		spaces = ((i < 0 || i >= width || GetMapData(Point(x, i), abs) % 2) ? 0 : spaces+1);

	return spaces < 3;
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
	return !!(point.first >= 0 && point.second >= 0 && point.first <= height && point.second <= width);
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
			Point end(i++, 0);
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
		if(!SpaceIsWalkable(start, false))
		{
			Point end(i++, width-1);
			for(; end.first < height; end.first++)
			{
				if(SpaceIsWalkable(end, false))
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
		if(!SpaceIsWalkable(start, false))
		{
			Point end(0, i++);
			for(; end.second < width; end.second++)
			{
				if(SpaceIsWalkable(end, false))
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
		if(!SpaceIsWalkable(start, false))
		{
			Point end(height-1, i++);
			for(end.second++; end.second < width; end.second++)
			{
				if(SpaceIsWalkable(end, false))
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
		int dx = end.first - start.first, dy = end.second - start.second;
		int cx = 0, cy = 0;

		if(dx > 0)
			if((dx % 2) == 1)
				cx = start.first + ((dx - (dx % 2)) / 2);
			else
				cx = start.first + (dx / 2);

		if(dy > 0)
			if((dy % 2) == 1)
				cy = start.second + ((dy - (dy % 2)) / 2);
			else
				cy = start.second + (dy / 2);

		Point center(cx ? cx : start.first, cy ? cy : start.second);
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
				int roomx = room->dwPosX * 5, roomy = room->dwPosY * 5;
				int roomsx = roomx + room->dwSizeX, roomsy = roomy + room->dwSizeY;

				for(PointList::iterator it = centers.begin(); it != centers.end(); it++)
				{
					if(it->first >= roomx && it->first <= roomsx && it->second >= roomy && it->second <= roomsy)
					{
						Exit exit(*it, pRooms[i]->pLevel->dwLevelNo, Linkage, 0);
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
	return !(SpaceHasFlag(D2Map::BlockWalk, point, abs) || SpaceHasFlag(D2Map::BlockPlayer, point, abs) ||
			 SpaceHasFlag(D2Map::ClosedDoor, point, abs) || SpaceHasFlag(D2Map::NPCCollision, point, abs) ||
			 SpaceHasFlag(D2Map::Object, point, abs));
}
bool D2Map::SpaceHasLineOfSight(const Point& point, bool abs) const { return !SpaceHasFlag(D2Map::BlockLineOfSight, point, abs); }
bool D2Map::SpaceIsInvalid(const Point& point, bool abs) const { return SpaceHasFlag(0xFFF, point, abs); }

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
	fprintf(f, "Map (%d x %d) at (%d, %d) for area %d:\r\n", width, height, posX, posY, level->dwLevelNo);

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			Point pt(i, j);
			char c = ' ';
			PointList::const_iterator begin = points.begin(), end = points.end();
			if(std::find(begin, end, pt) != end)
				c = 'X';
			else if(SpaceIsInvalid(pt, false))
				c = '?';
			else if(SpaceIsWalkable(pt, false))
				c = 'W';
			else if(SpaceHasLineOfSight(pt, false))
				c = '.';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\n");
	}
	fclose(f);
}

}
