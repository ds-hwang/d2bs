#pragma once

#include "PathReducer.h"

namespace Mapping
{
namespace Pathing
{
namespace Reducing
{

class NoPathReducer : public PathReducer
{
private:
	D2Map* map;
public:
	NoPathReducer(D2Map* map_) : map(map_) {}
	// the path is not reduced at all
	inline void Reduce(PointList const & in, PointList& out, bool abs) { out = in; }
	// accept only walkable nodes
	inline bool Reject(Point const & pt, bool abs) {
		return map->SpaceHasFlag(D2Map::Avoid, pt, abs) ||
			   map->SpaceHasFlag(D2Map::BlockWalk, pt, abs) ||
			   map->SpaceHasFlag(D2Map::BlockPlayer, pt, abs);
	}
};

}
}
}