#pragma once

#include <math3/math.h>
#include <map>

using vec3f = math3::vec3f;

struct PathNode
{
	vec3f startPt,endPt;	// target position
	int start,end;		// vertex pair
	PathNode():startPt(0.f),endPt(0.f),start(-1),end(-1){}
	PathNode(const PathNode &node):startPt(node.startPt),endPt(node.endPt),start(node.start),end(node.end){}
};

class Graph
{
public:
	typedef std::vector<PathNode> Path;				// destination at front(). First waypoint is at back().

	typedef std::multimap<int,int> Links;			// graph edges in flexible form

	Links links;				// graph edges
	std::vector<float> fastLinks;	// graph in matrix form. Usefull for pathfinding
	bool updateLinks;			// is any need to update fastLinks array
	std::vector<vec3f> points;		// graph points

	// remove all points and edges
	void clear();

	// find nearest graph point
	int getNearestPoint(const vec3f &pt)const;

	// build matrix representation of graph edges
	void cookLinks();

	// Simple wavefront pathfinding
	int path(int from,int to, Path &path);

	// connect two points
	void connect(int start,int end);

	// create directed edge
	void connectSingle(int start,int end);

	// add point to graph
	int addPoint(const vec3f &point);

	// distance between two graph points
	float distance(int start,int end)const
	{
		assert(validPoint(start));
		assert(validPoint(end));
		return vecLength(points[start]-points[end]);
	}

	bool validPoint(int pt)const
	{
		return pt < points.size();
	}

	bool connected(int start,int end)const
	{
		assert(!updateLinks);
		assert(validPoint(start));
		assert(validPoint(end));
		return fastLinks[start+end*points.size()] > 0.f;
	}

	int getEdgesCount()const
	{
		return links.size();
	}

	int getPointsCount()const
	{
		return points.size();
	}
};
