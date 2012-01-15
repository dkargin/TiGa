#include "stdafx.h"
//#include "traffic.h"
#include "navGraph.h"

///////////////////////////////////////////////////////////////////////////
// Graph implementation
///////////////////////////////////////////////////////////////////////////
void Graph::clear()
{
	points.clear();
	links.clear();
	fastLinks.clear();
	points.clear();
	updateLinks=false;
}

// find nearest graph point
int Graph::getNearestPoint(const vec3 &pt)const
{
	float minRange=vecLength(pt-points[0]);
	int best=0;
	for(int i=1;i<points.size();i++)
	{
		float range=vecLength(pt-points[i]);
		if(range<minRange)
		{
			minRange=range;
			best=i;
		}
	}
	return best;
}
// build matrix representation of graph edges
void Graph::cookLinks()
{
	if(!updateLinks)return;
	size_type size = points.size();
	fastLinks.resize(size * size,0);
	for(Links::iterator it = links.begin();it != links.end();++it)
		fastLinks[size*it->first + it->second] = distance(it->first, it->second);
	updateLinks=false;
}
// connect two points
void Graph::connect(int start,int end)
{
	links.insert(make_pair(start,end));
	links.insert(make_pair(end,start));
	updateLinks=true;
}

void Graph::connectSingle(int start,int end)
{
	links.insert(make_pair(start,end));
	updateLinks=true;
}

// add point to graph
int Graph::addPoint(const vec3 &point)
{
	//std::tr1::function
	updateLinks=true;
	points.push_back(point);
	return points.size();
}

// Simple wavefront pathfinding
int Graph::path(int from, int to, Path &path)
{
	assert(validPoint(from));
	assert(validPoint(to));
	cookLinks();
	// Pathfinding tree node
	struct TreeNode
	{
		bool open;	// if node is in wave frontier
		int back;	// index of previous node
		float cost;	// total path cost
		TreeNode():back(-1),cost(0),open(false){}
		TreeNode(const TreeNode &node):open(node.open),cost(node.cost),back(node.back){}
	};
	size_type size=points.size();
	vector<TreeNode> pathTree(points.size());	// pathfinding tree

	pathTree[from].back=from;	
	pathTree[from].open=true;

	TreeNode *tree=&pathTree.front();	// just for debug view

	while(true)
	{
		size_type best=-1;	// best node index
		// 1. Find best node
		for(size_type i=0;i<size;i++)
			if(pathTree[i].open && (best==-1 || pathTree[i].cost<pathTree[best].cost))
				best=i;
		// 2. Check if we reached the goal
		if(best==to)
			break;
		// 3. Check if the wave is empty. No path returned
		if(best==-1)
			return -1;	// wave is empty
		// 4. Expand path tree
		TreeNode &bestNode=pathTree[best];
		for(size_type i=0;i<size;i++)	
		{
			TreeNode &adjacent=pathTree[i];
			float linkCost=fastLinks[best*size+i];	
			if(linkCost<=0.0f)continue;	// ignore zero and negative costs
			float newCost=bestNode.cost+linkCost;
			if(adjacent.back==-1 || newCost<adjacent.cost)
			{
				adjacent.cost=newCost;
				adjacent.back=best;
				adjacent.open=true;
			}
		}
		bestNode.open=false;
	}
	// build path
	int current=to;
	while(current!=from)
	{
		PathNode node;
		node.start=pathTree[current].back;
		node.end=current;
		node.startPt=points[node.start];
		node.endPt=points[node.end];		
		
		current=node.start;
		path.push_back(node);
	}
	return 1;
}