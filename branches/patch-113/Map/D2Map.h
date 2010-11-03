#pragma once

#include "Map.h"

#include <list>

#include "D2Structs.h"
#include "D2Ptrs.h"

namespace Mapping
{

class D2Map : public Map
{
private:
	enum CollisionFlag {
		None				= 0x00000000,
		BlockWalk			= 0x00000001,
		BlockLineOfSight	= 0x00000002,
		Wall				= 0x00000004,
		BlockPlayer			= 0x00000008,
		AlternateTile		= 0x00000010,
		Blank				= 0x00000020,
		Missile				= 0x00000040,
		Player				= 0x00000080,
		NPCLocation			= 0x00000100,
		Item				= 0x00000200,
		Object				= 0x00000400,
		ClosedDoor			= 0x00000800,
		NPCCollision		= 0x00001000,
		FriendlyNPC			= 0x00002000,
		Unknown				= 0x00004000,
		DeadBody			= 0x00008000 // also portal
	};

	Act* act;
	const Level* level;
	int width, height;
	int posX, posY;
	Matrix<CollisionFlag>* mapPoints;
	CRITICAL_SECTION* lock;

	typedef std::list<Room2*> RoomList;

	void Build(void);
	inline void AddRoomData(Room2* room) { D2COMMON_AddRoomData(act, level->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1); }
	inline void RemoveRoomData(Room2* room) { D2COMMON_RemoveRoomData(act, level->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1); }

	void AddRoom(Room2* const room, RoomList& rooms, UnitAny* player);
	void AddCollisionMap(const CollMap* const map);
	void SetCollisionData(int x, int y, WORD value);
	bool IsGap(int x, int y, bool abs) const;
	void FillGaps(void);
	void ShrinkMap(void);

public:
	D2Map(Act* act, const Level* level);
	~D2Map(void);

	void Dump(const char* file, const PointList& points) const;

	Point AbsToRelative(const Point& point) const;
	Point RelativeToAbs(const Point& point) const;

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

	int GetMapData(const Point& point, bool abs = true) const;
	bool IsValidPoint(const Point& point, bool abs = true) const;

	bool SpaceHasFlag(int flag, const Point& point, bool abs = true) const;
	bool PathHasFlag(int flag, const PointList& points, bool abs = true) const;

	bool SpaceIsWalkable(const Point& point, bool abs = true) const;
	bool SpaceHasLineOfSight(const Point& point, bool abs = true) const;
	bool SpaceIsInvalid(const Point& point, bool abs = true) const;

	bool PathIsWalkable(const PointList& points, bool abs = true) const;
	bool PathHasLineOfSight(const PointList& points, bool abs = true) const;
};

}
