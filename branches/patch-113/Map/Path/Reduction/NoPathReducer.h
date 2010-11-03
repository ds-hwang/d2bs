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
public:
	// the path is not reduced at all
	inline void Reduce(PointList const & in, PointList& out, bool abs) { out = in; }
	// accept all nodes
	inline bool Reject(Point const & pt, bool abs) { return false; }
};

}
}
}