#pragma once

#include <vector>
#include <utility>
#include <assert.h>

namespace Mapping
{

typedef std::pair<int, int> Point;
typedef std::vector<Point> PointList;

class Map
{
public:
	virtual void Dump(const char* file, const PointList& points) const = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	virtual int GetMapData(const Point& point, bool abs) const = 0;
	virtual bool IsValidPoint(const Point& point, bool abs) const = 0;

	virtual bool SpaceHasFlag(int flag, const Point& point, bool abs) const = 0;
	virtual bool PathHasFlag(int flag, const PointList& points, bool abs) const = 0;
};

template <typename T>
class Matrix
{
private:
	typedef std::vector<T> TList;

	std::vector<TList> points;
	int width, height;

public:
	Matrix() : width(0), height(0) {}
	Matrix(int width, int height) {
		this->width = width; this->height = height;
		points = std::vector<TList>(width);
		for(int i = 0; i < width; i++)
			points[i] = TList(height);
	}
	T __fastcall GetPoint(int x, int y) const {
		assert(x < width && y < height);
		return points[x][y];
	}
	void SetPoint(int x, int y, T value) {
		assert(x < width && y < height);
		points[x][y] = value;
	}
	void __fastcall RemoveRow(int row) {
		assert(row < height);
		// TODO: implement.
		std::vector<TList>::iterator it = points.begin()+row;
		points.erase(it);
	}
	void __fastcall RemoveColumn(int column) {
		assert(column < width);
		// TODO: implement.
		for(int i = 0; i < width; i++)
		{
			TList::iterator it = points[i].begin()+column;
			points[i].erase(it);
		}
	}
};

}
