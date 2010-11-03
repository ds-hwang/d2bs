#pragma once

#include "..\Map.h"
#include "Reduction\PathReducer.h"

namespace Mapping
{
namespace Pathing
{

typedef int (__fastcall *Estimator)(Map const &, Point const &, Point const &);
typedef int (__fastcall *Distance)(Point const &, Point const &);

class Path
{
public:
	virtual void GetPath(Point const & start, Point const & end, PointList& list, bool abs) = 0;

	virtual void SetMap(Map& map) = 0;
	virtual void SetPathReducer(Reducing::PathReducer* reducer) = 0;
};

}
}