#include "Map.h"
#include "Functions.h"
#include "D2Utilities.h"
#include <algorithm>
#include <assert.h>
#include "Script.h"

namespace Pathing
{

Map::Map(int areaId, bool buildMap)
{
	InitializeCriticalSection(&lock);
	this->areaId = areaId;
	built = false;
	if(buildMap)
		Build();
}

Map::~Map(void)
{
	width = height = 0;
	DestroyArray();
	DeleteCriticalSection(&lock);
}

void Map::BuildArray(int height, int width)
{
	mapPoints = new WORD*[height];
	for(int i = 0; i < height; i++)
		mapPoints[i] = new WORD[width];

	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			mapPoints[i][j] = 0xFFFF;
}

void Map::DestroyArray(void)
{
	for(int i = 0; i < height; i++)
		delete[] mapPoints[i];

	delete[] mapPoints;
}

void Map::Build(void)
{
	// get the map size
	Level* level = GetLevel(this->areaId);
	width = level->dwSizeX * 5;
	height =  level->dwSizeY * 5;
	posX = level->dwPosX * 5;
	posY = level->dwPosY * 5;
	UnitAny* player = GetPlayerUnit();

	// everything beyond here must be sync'd
	EnterCriticalSection(&lock);
	// build the array to hold the map data
	BuildArray(height, width);

	// add the first room in the level
	RoomList addedRooms = RoomList();
	AddRoom(level->pRoom2First, level, player, addedRooms);

	built = true;
	// TODO: figure out how to design this to be able to remove rows/cols
//	ShrinkMap();
	FillGaps();

	LeaveCriticalSection(&lock);
}

void Map::AddRoom(Room2* const room, const Level* const level,
				  const UnitAny* const player, RoomList& addedRooms)
{
	if(!room || !level || !player || !player->pAct || !player->pPath || room->pLevel->dwLevelNo != areaId)
		return;

	// ignore rooms we've seen before
	if(std::find(addedRooms.begin(), addedRooms.end(), room) != addedRooms.end())
		return;

	addedRooms.push_back(room);

	// add the room if necessary
	bool added = false;
	if(!room->pRoom1)
	{
		AddRoomData(player->pAct, room->pLevel->dwLevelNo, room->dwPosX, room->dwPosY, player->pPath->pRoom1);
		added = true;
	}

	// add the collision data to the map
	if(room->pRoom1)
		AddCollisionMap(room->pRoom1->Coll);

	// now do the same to every room near this one
	Room2** rooms = room->pRoom2Near;
	for(DWORD i = 0; i < room->dwRoomsNear; i++)
		AddRoom(rooms[i], level, player, addedRooms);

	// and remove the room afterwards if we added it
	if(added)
		RemoveRoomData(player->pAct, room->pLevel->dwLevelNo, room->dwPosX, room->dwPosY, player->pPath->pRoom1);
}

void Map::AddCollisionMap(const CollMap* const map)
{
	if(map == NULL)
		return;

	int	x = map->dwPosGameX - posX,
		y = map->dwPosGameY - posY,
		dx = map->dwPosGameX + map->dwSizeGameX - posX,
		dy = map->dwPosGameY + map->dwSizeGameY - posY;

	WORD* p = map->pMapStart;

	// walk over the collision map, adding each point
	// the map is constructed backwards, so we must walk over it from y to x
	for(int i = y; i < dy && p != map->pMapEnd; i++)
		for(int j = x; j < dx && p != map->pMapEnd; j++, p++)
			SetCollisionData(i, j, *p);
}

void Map::SetCollisionData(int x, int y, WORD value)
{
	// sync this to ensure that we don't try to add to a dead map or something
	EnterCriticalSection(&lock);

	assert((x >= 0 && y >= 0 && x < height && y < width));
	mapPoints[x][y] = value;

	LeaveCriticalSection(&lock);
}

bool Map::IsGap(int x, int y)
{
	if(GetCollisionData(x, y) % 2)
		return false;

	int spaces = 0;

	for(int i = x-2; i <= x+2 && spaces < 3; i++)
		spaces = ((i < 0 || i >= height || GetCollisionData(i, y) % 2) ? 0 : spaces+1);

	if(spaces < 3)
		return true;

	for(int i = y-2, spaces = 0; i <= y+2 && spaces < 3; i++)
		spaces = ((i < 0 || i >= width || GetCollisionData(x, i) % 2) ? 0 : spaces+1);

	return spaces < 3;
}

void Map::FillGaps(void)
{
	// don't try to fill gaps on a dead map
	if(!built)
		Build();

	EnterCriticalSection(&lock);

	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(IsGap(i, j))
				SetCollisionData(i, j, 2);

	LeaveCriticalSection(&lock);
}

void Map::ShrinkMap(void)
{
	if(!built)
		Build();

	EnterCriticalSection(&lock);

	bool blankRow = true;
	// walk over the rows, finding all blank (fully unwalkable) ones
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
			if(SpaceIsWalkable(i, j))
				blankRow = false;
		if(blankRow)
		{
			// remove that row from the map, since it's irrelevant
		}
	}

	bool blankCol = true;
	// walk over the columns, finding all blank (fully unwalkable) ones
	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
			if(SpaceIsWalkable(j, i))
				blankCol = false;
		if(blankCol)
		{
			// remove that column from the map, since it's irrelevant
		}
	}

	LeaveCriticalSection(&lock);
}

WORD Map::GetCollisionData(WORD x, WORD y)
{
	if(!built)
		Build();

	// sync this to ensure the map is either built or dead
	EnterCriticalSection(&lock);

	assert((x >= 0 && y >= 0 && x < height && y < width));
	WORD value = mapPoints[x][y];

	LeaveCriticalSection(&lock);

	return value;
}

bool Map::SpaceHasFlag(CollFlags flag, WORD x, WORD y) { return ((GetCollisionData(x, y) & flag) == flag); }
bool Map::SpaceIsWalkable(WORD x, WORD y) { return SpaceHasFlag(Map::Walkable, x, y) || SpaceHasFlag(Map::PlayerWalkable, x, y); }
bool Map::SpaceHasLineOfSight(WORD x, WORD y) { return SpaceHasFlag(Map::LineOfSight, x, y); }
bool Map::SpaceIsInvalid(WORD x, WORD y) { return GetCollisionData(x, y) == 0xFFF; }

bool Map::SpaceHasFlag(CollFlags flag, POINT point) { return SpaceHasFlag(flag, (WORD)point.x, (WORD)point.y); }
bool Map::SpaceIsWalkable(POINT point) { return SpaceIsWalkable((WORD)point.x, (WORD)point.y); }
bool Map::SpaceHasLineOfSight(POINT point) { return SpaceHasLineOfSight((WORD)point.x, (WORD)point.y); }
bool Map::SpaceIsInvalid(POINT point) { return SpaceIsInvalid((WORD)point.x, (WORD)point.y); }

bool Map::PathIsWalkable(MapPointList points)
{
	for(MapPointList::iterator it = points.begin(); it != points.end(); it++)
		if(!SpaceIsWalkable(it->point))
			return false;
	return true;
}

bool Map::PathHasLineOfSight(MapPointList points)
{
	for(MapPointList::iterator it = points.begin(); it != points.end(); it++)
		if(!SpaceHasLineOfSight(it->point))
			return false;
	return true;
}

void Map::Dump(void)
{
	char map[1024];
	sprintf(map, "%s\\map-%d.txt", Script::GetScriptPath(), areaId);
	FILE* f = fopen(map, "wt");
	fprintf(f, "Map (%d x %d, id %d) for area %d:\r\n", width, height, GetMapId(), areaId);

	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			WORD data = GetCollisionData(i, j);
			char c = ' ';
			if(SpaceIsWalkable(i, j))
				c = 'X';
			else if(SpaceIsInvalid(i, j))
				c = '?';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\r\n");
	}
	fclose(f);
}

}
