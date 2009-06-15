#include "Map.h"
#include "Functions.h"
#include "D2Utilities.h"
#include <algorithm>
#include <assert.h>
#include "Script.h"

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
	FillGaps();
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
	for(int j = x; j < dx && p != map->pMapEnd; j++)
		for(int i = y; i < dy && p != map->pMapEnd; i++, p++)
			SetCollisionData(i, j, *p);
}

void Map::SetCollisionData(int x, int y, WORD value)
{
	// sync this to ensure that we don't try to add to a dead map or something
	EnterCriticalSection(&lock);

	assert((x >= 0 && y >= 0 && x < height && y < width));
	if(value == 0x400) value = 0x2b6b;
	if(std::find(collisionValues.begin(), collisionValues.end(), value) == collisionValues.end())
		collisionValues.push_back(value);
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

WORD Map::GetCollisionData(int x, int y) const
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

void Map::Dump(void) const
{
	char map[1024];
	sprintf(map, "%s\\map-%d.txt", Script::GetScriptPath(), areaId);
	FILE* f = fopen(map, "wt");
	fprintf(f, "Map (%d x %d, id %d) for area %d:\r\nCollision values:\r\n", width, height, GetMapId(), areaId);
	std::sort(collisionValues.begin(), collisionValues.end());

	for(WordList::iterator it = collisionValues.begin(); it != collisionValues.end(); it++)
		fprintf(f, "0x%X\r\n", *it);

	fprintf(f, "\r\n\r\n");
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			WORD data = GetCollisionData(i, j);
			char c = 'X';
			if(data % 2)
				c = ' ';
			else if(data == 0x2B6B)
				c = '.';
			else if(data == 0xFFFF)
				c = '?';
			fprintf(f, "%c", c);
		}
		fprintf(f, "\r\n");
	}
	fclose(f);
}
