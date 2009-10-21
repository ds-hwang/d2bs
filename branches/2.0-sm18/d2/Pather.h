#pragma once
#ifndef __PATHER_H__
#define __PATHER_H__

#include <windows.h>
#include <list>

namespace Pathing
{

struct MapPoint
{
	POINT point;
	WORD area;
	MapPoint() { point.x = point.y = -1; area = -1; }
	MapPoint(int x, int y, int area) { point.x = x; point.y = y; this->area = area; }
	MapPoint(const MapPoint& old) { point.x = old.point.x; point.y = old.point.y; area = old.area; }
	bool operator==(const MapPoint& other) const
	{
		return point.x == other.point.x && point.y == other.point.y && area == other.area;
	}
};

enum ReductionType
{
	Walk,
	Teleport
	// TODO: Fill this with other reduction types (if necessary)
};

typedef std::list<POINT> PointList;
typedef std::list<MapPoint> MapPointList;

class Pather
{
protected:
	ReductionType reductionType;

public:
	Pather(ReductionType type)
	{
		this->reductionType = type;
	}

	virtual bool Search(const MapPoint start, const MapPoint end) = 0;

	// these are too pathing algorithm-dependent to move to a separate class...
	virtual void Initialize(void) = 0;
	virtual void Reset(void) = 0;
	virtual const MapPoint GetPrevPoint(void) = 0;
	virtual const MapPoint GetNextPoint(void) = 0;
	virtual const MapPointList GetAllPoints(void) = 0;
};

}

#endif
