#pragma once

#include "Map.h"

#include <list>
#include <map>

#include "D2Structs.h"
#include "D2Ptrs.h"

namespace Mapping
{

enum ExitType
{
	Linkage = 1,
	Tile = 2
};

struct Exit
{
public:
	DWORD Target;
	Point Position;
	ExitType Type;
	DWORD TileId;

	Exit(Point location, DWORD levelId, ExitType type, DWORD tileId) :
		Target(levelId), Position(location), Type(type), TileId(tileId) {}
};

class D2Map;

typedef std::vector<Exit> ExitArray;
typedef std::map<DWORD, D2Map*> MapList;

class D2Map : public Map
{
public:
	enum CollisionFlag {
		None				= 0x0000,
		BlockWalk			= 0x0001,
		BlockLineOfSight	= 0x0002,
		Wall				= 0x0004,
		BlockPlayer			= 0x0008,
		AlternateTile		= 0x0010,
		Blank				= 0x0020,
		Missile				= 0x0040,
		Player				= 0x0080,
		NPCLocation			= 0x0100,
		Item				= 0x0200,
		Object				= 0x0400,
		ClosedDoor			= 0x0800,
		NPCCollision		= 0x1000,
		FriendlyNPC			= 0x2000,
		Unknown				= 0x4000,
		DeadBody			= 0x8000, // also portal
		ThickenedWall		= 0xFEFE,
		Avoid				= 0xFFFF
	};

private:
	typedef std::list<Room2*> RoomList;

	static MapList cache;

	Act* act;
	const Level* level;
	int width, height;
	int posX, posY, endX, endY;
	Matrix<CollisionFlag>* mapPoints;
	CRITICAL_SECTION* lock;

	void Build(void);
	inline void AddRoomData(Room2* room) const
	{ D2COMMON_AddRoomData(room->pLevel->pMisc->pAct, room->pLevel->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1); }
	inline void RemoveRoomData(Room2* room) const
	{ D2COMMON_RemoveRoomData(room->pLevel->pMisc->pAct, room->pLevel->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1); }

	void AddRoom(Room2* const room, RoomList& rooms, UnitAny* player);
	void AddCollisionMap(const CollMap* const map);
	void SetCollisionData(int x, int y, WORD value);
	bool IsGap(int x, int y, bool abs) const;
	void FillGaps(void);
	void ShrinkMap(void);
	void ThickenWalls(void);
	D2Map(const Level* level);
	~D2Map(void);

public:

	static D2Map* GetMap(Level* level);
	static void ClearCache(void);
	void Dump(const char* file, const PointList& points) const;

	Point AbsToRelative(const Point& point) const;
	Point RelativeToAbs(const Point& point) const;

	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }

	int GetMapData(const Point& point, bool abs = true) const;
	bool IsValidPoint(const Point& point, bool abs = true) const;

	void GetExits(ExitArray&) const;

	bool SpaceHasFlag(int flag, const Point& point, bool abs = true) const;
	bool PathHasFlag(int flag, const PointList& points, bool abs = true) const;

	bool SpaceIsWalkable(const Point& point, bool abs = true) const;
	bool SpaceHasLineOfSight(const Point& point, bool abs = true) const;
	bool SpaceIsInvalid(const Point& point, bool abs = true) const;

	bool PathIsWalkable(const PointList& points, bool abs = true) const;
	bool PathHasLineOfSight(const PointList& points, bool abs = true) const;
};

}
