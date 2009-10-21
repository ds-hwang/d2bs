#pragma once
#ifndef __MAP_H__

#include <windows.h>
#include <list>
#include <vector>

#include "Structs.h"
#include "Pointers.h"
#include "Pather.h"

namespace Pathing
{

class Map;

typedef std::list<Room2*> RoomList;
typedef std::list<Map*> MapList;

// each Map will represent a single level map and will
// have a list of connected Maps
class Map
{
private:
	enum CollFlags {
		Walkable			= 0x1,
		LineOfSight			= 0x2,
		Ranged				= 0x4,
		PlayerWalkable		= 0x8,
		Exit				= 0x10,
		Light				= 0x20,
		Unknown1			= 0x40,
		Player				= 0x80,
		NPC					= 0x100,
		Item				= 0x200,
		Object				= 0x400,
		ClosedDoor			= 0x800,
		Unknown2			= 0x1000,
		FriendlyNPC			= 0x2000,
		Unknown3			= 0x4000,
		DeadBody			= 0x8000
	};

	int areaId;
	int width, height;
	int posX, posY;
	bool built;
	WORD **mapPoints;
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
	void ShrinkMap(void);

public:
	Map(int areaId, bool buildMap = false);
	~Map(void);

	static DWORD GetMapId(void) { return GetD2ClientMapId(); }
	void Dump(void);

	WORD GetCollisionData(WORD x, WORD y);

	bool SpaceHasFlag(CollFlags flag, WORD x, WORD y);
	bool SpaceIsWalkable(WORD x, WORD y);
	bool SpaceHasLineOfSight(WORD x, WORD y);
	bool SpaceIsInvalid(WORD x, WORD y);

	bool SpaceHasFlag(CollFlags flag, POINT point);
	bool SpaceIsWalkable(POINT point);
	bool SpaceHasLineOfSight(POINT point);
	bool SpaceIsInvalid(POINT point);

	bool PathIsWalkable(MapPointList points);
	bool PathHasLineOfSight(MapPointList points);
};

}

#endif
