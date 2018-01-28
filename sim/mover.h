#pragma once

#include "device.h"
#include "gameObject.h"
#include "objectManager.h"

namespace sim
{

// Mover is a type of device, that allows game object to move
// Mover uses 'Driver' class to recieve movement commands and
// deal with kinematics
class Mover: public Device
{
	friend class ObjectManager;	
public:
	class Driver;
	friend class Driver;

	enum State
	{
		Idle,
		//Wait,
		Moving
	};

	struct Listener
	{
		virtual void onSetState(int state,float value)=0;
	}*listener;

	Mover(Mover* def);
	//int readDesc(IOBuffer &context);
	//int writeDesc(IOBuffer &context);	
	int readState(StreamIn &buffer);
	int writeState(StreamOut &buffer);
	
	virtual ~Mover();
	virtual void stop();
	virtual void update(float dt)=0;

	virtual void updateTask();				/// update the path
	virtual State getState() const;
	virtual Driver * getDriver();
	virtual const Driver * getDriver() const;
	virtual void setDriver(Driver * d);
	virtual void clearDriver();	

	virtual void directionControl(const Pose::vec& direction, float speed) = 0;
	virtual float getMaxVelocity(int dir) const { return 0.f;}

#ifdef FUCK_THIS

#ifdef DEVICE_RENDER
	virtual void render(HGE * hge);
#endif

	pathProject::PathProcess * pathProcess();
#endif
	Pose::vec getLastControl() const;

protected:	
	Pose::vec lastControl;
	Driver * driver;
	State state;

	/// From definition
	int adjacency;
	bool bounds;
	bool smooth;
	bool heuristic;
	//pathProject::PathProcess * pathProcess;	// for grid pathfinding
};


// Driver class
// The one, that drivers 'mover' object
// Contains a part of AI that deals with movement control
class Mover::Driver
{
public:
	enum PathEvent
	{
		taskNew,
		taskSuccess,
		taskFail,
	};

	Driver(Mover* mover);
	virtual ~Driver(){};

	virtual void update(float dt);
	virtual void updatePath(float dt);		/// update current waypoint
	virtual void updateTask();				/// update current path

	virtual void render(Fx::RenderContext* hge);

	virtual int obstaclesAdd(const Trajectory2& tr, float size);	// adds object to evade
	virtual void obstaclesClear();

	virtual void walk(const Pose& pose);
	virtual void walk(const Pose::pos & target);				// set moving target
	virtual void face(const Pose::vec &t);

	virtual Mover * getMover() const {return mover;}

	class Listener
	{
	public:
		virtual void moverEvent(Mover *mover,int state)=0;
	}*listener;

	float rayCast(const Pose::vec& offset,const Pose::vec& dir)const;

	int getWaypointsCount() const;
	int getWaypoints(vec2f * points, int max = -1) const;				// copy local waypoints to "points" array

	virtual float pathLength();		// current path length
	virtual float timeToImpact();	// time to impact in described obstacle
	Pose::vec pathDirection();
	Pose::vec pathError();
	
protected:
	typedef std::vector<Pose> Path;
	Path path;
	bool waiting;

	virtual void getPath(Path & ptr, bool success);
	/*
	struct Waypoint2
	{
		vec2f v;
		int action;
	};
	std::vector<Waypoint2> path;*/
	typedef std::pair<Trajectory2, float> Obstacle;
	typedef std::vector< Obstacle > Obstacles;	
	Obstacles obstacles;

	Pose::pos targetDir;
	Pose::pos target;
	struct MoveTask
	{
		bool target;
		bool dir;
		MoveTask();
		void clear();
	}task;

	uint32 pathCurrent;		
	float minDistance;
	Mover * mover;
protected:
	//int buildPath();
	bool tryNextWaypoint();					// try to get next waypoint
	const Pose& getCurrentWaypoint();
	void clearTask();
//	pathProject::PathProcess * pathProcess();
};

Mover::Driver * createFlockingDriver(Mover* m);
Mover::Driver * createVODriver(Mover* m);
Mover::Driver * createVO2Driver(Mover* m);

void drawArrow(Fx::RenderContext* rc,const vec2f &from,const vec2f &to,Fx::FxRawColor color,float width=0.01,float length=0.02);
void drawPoint(Fx::RenderContext* rc,const vec2f &pos, Fx::FxRawColor color, float size, int style=0);
void drawLine(Fx::RenderContext* rc,const vec2f &from,const vec2f &to,Fx::FxRawColor color);


} // namesoace sim
