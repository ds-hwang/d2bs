#pragma once
#ifndef __MAP_H__

#include <windows.h>
#include <list>
#include <vector>

#include "Structs.h"
#include "Pointers.h"

class Map;

typedef std::list<Room2*> RoomList;
typedef std::list<Map*> MapList;
typedef std::vector<WORD> WordList;

// each Map will represent a single level map and will
// have a list of connected Maps
class Map
{
private:
	int areaId;
	int width, height;
	int posX, posY;
	bool built;
	WORD **mapPoints;
	WordList collisionValues;
	CRITICAL_SECTION lock;

	void Build(void);
	void BuildArray(int height, int width);
	void DestroyArray(void);
	void AddRoom(Room2* const room, const Level* const level,
					const UnitAny* const player, RoomList& addedRooms);
	void AddCollisionMap(const CollMap* const map);
	void SetCollisionData(int x, int y, WORD value);
	bool IsGap(int x, int y);
	void FillGaps(void);

public:
	Map(int areaId, bool buildMap = false);
	~Map(void);

	WORD GetCollisionData(int x, int y);
	static DWORD GetMapId(void) { return GetD2ClientMapId(); }
	void Dump(void);
};

#endif
