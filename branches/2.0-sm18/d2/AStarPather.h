#pragma once
#ifndef __ASTARPATHER_H__
#define __ASTARPATHER_H__

#include "Pather.h"
#include <list>
#include <functional>

namespace Pathing
{

struct Node : public std::binary_function<Node, Node, bool>
{
	const MapPoint* point;
	Node* parent;
	int cost;
	Node() : point(NULL) { point = NULL; parent = NULL; cost = -1; }
	Node(const MapPoint* p, Node* parent) : point(p)
	{
		this->parent = parent;
		this->cost = 0;
	}
	bool operator()(const Node& left, const Node& right) const
	{
		return left.cost < right.cost;
	}
	bool operator==(const Node& other) const
	{
		return point == other.point && parent == other.parent && cost == other.cost;
	}
};

typedef std::list<Node> NodeList;

class AStarPather : Pather
{
private:
	int h(const Node& x);
	int g(const Node& x);
	inline int f(const Node& x) { return g(x) + h(x); }

	NodeList currentList;
	NodeList::iterator currentIterator;

public:
	virtual bool Search(const MapPoint start, const MapPoint end);

	virtual void Initialize(void);
	virtual void Reset(void);
	virtual const MapPoint GetPrevPoint(void);
	virtual const MapPoint GetNextPoint(void);
	virtual const MapPointList GetAllPoints(void);
};

}

#endif
