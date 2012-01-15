#pragma once
using namespace std;

struct PathNode
{
	vec3 startPt,endPt;	// target position
	int start,end;		// vertex pair
	PathNode():startPt(0.f),endPt(0.f),start(-1),end(-1){}
	PathNode(const PathNode &node):startPt(node.startPt),endPt(node.endPt),start(node.start),end(node.end){}
};

class Graph
{
public:
	typedef vector<PathNode> Path;				// destination at front(). First waypoint is at back().

	typedef multimap<int,int> Links;			// graph edges in flexible form
	typedef vector<float>::size_type size_type;	
	
	Links links;				// graph edges
	vector<float> fastLinks;	// graph in matrix form. Usefull for pathfinding
	bool updateLinks;			// is any need to update fastLinks array
	vector<vec3> points;		// graph points

	// remove all points and edges
	void clear();	
	// find nearest graph point
	int getNearestPoint(const vec3 &pt)const;
	// build matrix representation of graph edges
	void cookLinks();
	// Simple wavefront pathfinding
	int path(int from,int to,Path &path);
	// connect two points
	void connect(int start,int end);
	// create directed edge
	void connectSingle(int start,int end);
	// add point to graph
	int addPoint(const vec3 &point);
	// distance between two graph points
	inline float distance(int start,int end)const
	{
		assert(validPoint(start));
		assert(validPoint(end));
		return vecLength(points[start]-points[end]);
	}
	inline bool validPoint(size_type pt)const
	{
		return pt<points.size();
	}
	inline bool connected(size_type start,size_type end)const
	{
		assert(!updateLinks);
		assert(validPoint(start));
		assert(validPoint(end));
		return fastLinks[start+end*points.size()]>0.f;
	}
	inline int getEdgesCount()const
	{
		return links.size();
	}
	inline int getPointsCount()const
	{
		return points.size();
	}
};