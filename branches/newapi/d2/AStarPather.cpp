#include "AStarPather.h"
#include <queue>
#include <vector>

namespace Pathing
{

int AStarPather::h(const Node& x)
{
	// TODO: Make this look up the data from the enemy table and the map data
	// to calculate a value for the node's move-to cost
	// ALSO: Come up with toggle switches for enemy data and other potential
	// move-to cost heuristics
	return -1;
}

int AStarPather::g(const Node& x)
{
	int r = 0;
	Node* ptr = &(const_cast<Node&>(x));
	while(ptr->parent != NULL)
	{
		r += h(*ptr);
		ptr = ptr->parent;
	}
	return r;
}

void ExpandNode(Node& p, std::vector<Node>& list)
{
	list.clear();
	// expanding a node simply consists of generating a list of the 8 corners of the node
	// and calculating the cost for each corner
	list[0] = Node(new MapPoint(p.point->point.x+1, p.point->point.y, p.point->area), &p);
	list[1] = Node(new MapPoint(p.point->point.x-1, p.point->point.y, p.point->area), &p);
	list[2] = Node(new MapPoint(p.point->point.x, p.point->point.y+1, p.point->area), &p);
	list[3] = Node(new MapPoint(p.point->point.x, p.point->point.y-1, p.point->area), &p);
	list[4] = Node(new MapPoint(p.point->point.x+1, p.point->point.y-1, p.point->area), &p);
	list[5] = Node(new MapPoint(p.point->point.x+1, p.point->point.y+1, p.point->area), &p);
	list[6] = Node(new MapPoint(p.point->point.x-1, p.point->point.y+1, p.point->area), &p);
	list[7] = Node(new MapPoint(p.point->point.x-1, p.point->point.y-1, p.point->area), &p);
}

bool AStarPather::Search(const MapPoint start, const MapPoint end)
{
	// A slight modification on the A* algorithm -- the closed list is
	// instead a 'seen' list. Its' purpose is to hold all seen nodes,
	// instead of only those nodes who have been examined. This simplifies
	// the list search for adding examined nodes. The only list that must
	// be searched is the seen list.
	std::priority_queue<Node, std::vector<Node>, Node> open;
	NodeList seen;
	Node ns(&start, NULL);
	ns.cost = f(ns);
	Node& current = ns;
	open.push(ns);
	seen.push_back(ns);

	while(!open.empty())
	{
		current = open.top();
		if(*current.point == end)
		{
			// we found the goal -- reverse the list and return true
			currentList.push_front(current);
			while(current.parent != NULL)
				currentList.push_front(*current.parent);

			return true;
		}
		else
		{
			open.pop();
			std::vector<Node>& nodes = std::vector<Node>(8);
			ExpandNode(current, nodes);
			for(std::vector<Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
			{
				// see if we've already seen this node
				if(std::find(seen.begin(), seen.end(), *it) == seen.end())
				{
					// we haven't seen it, calculate its' cost
					it->cost = f(*it);
					// ignore nodes whose cost are negative, they can't be pathed to anyway
					if(it->cost > -1)
					{
						open.push(*it);
						seen.push_back(*it);
					}
				}
				delete it->point;
			}
		}
	}

	return false;
}


void AStarPather::Initialize(void)
{
	Reset();
}

void AStarPather::Reset(void)
{
	currentIterator = currentList.begin();
}

const MapPoint AStarPather::GetPrevPoint(void)
{
	if(currentIterator == currentList.begin())
		return MapPoint();
	return *(currentIterator--)->point;
}

const MapPoint AStarPather::GetNextPoint(void)
{
	if(currentIterator == currentList.end())
		return MapPoint();
	return *(currentIterator++)->point;
}

const MapPointList AStarPather::GetAllPoints(void)
{
	MapPointList list;
	Reset();
	MapPoint point = GetNextPoint();
	do {
		if(point.area == -1)
			break;
		list.push_back(point);
	} while((point = GetNextPoint()).area != -1);
	return list;
}


}
