#pragma once

#include "PathReducer.h"
#include "../Path.h"
#include "../../D2Map.h"

namespace Mapping
{
namespace Pathing
{
namespace Reducing
{

#pragma warning ( disable: 4512 )

class TeleportPathReducer : public PathReducer
{
private:
	D2Map& map;
	Distance distance;
	int range;

public:
	TeleportPathReducer(D2Map& m, Distance d, int _range = 20) : map(m), distance(d), range(_range*10) {}

	void Reduce(PointList const & in, PointList& out, bool abs)
	{
		PointList::const_iterator it = in.begin(), end = in.end();
		out.push_back(*it);
		while(it != end)
		{
			Point prev = *(out.end()-1);
			while(distance(*it, prev) < range) it++;
			it--;
			out.push_back(*it);
			it++;
		}
	}
	bool Reject(Point const & pt, bool abs)
	{
		return !map.SpaceIsWalkable(pt, abs);
	}
};

#pragma warning ( default: 4512 )

}
}
}
