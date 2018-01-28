#include "mover.h"
#include "unit.h"

namespace sim {

////////////////////////////////////////////////////////////////////
// Mover Tasks
////////////////////////////////////////////////////////////////////
Mover::Driver::MoveTask::MoveTask()
:target(false),dir(false)
{}
void Mover::Driver::MoveTask::clear()
{
	target=false;
	dir=false;
}
////////////////////////////////////////////////////////////////////
// Driver
////////////////////////////////////////////////////////////////////
Mover::Driver::Driver(Mover* mover)
:listener(NULL),pathCurrent(-1),minDistance(10),mover(mover), PathFinder::Client(&mover->getManager()->pathFinder)
{}

int Mover::Driver::obstaclesAdd(const Geom::Traectory2 &tr,float size)
{
	obstacles.push_back(std::make_pair(tr,size));
	return obstacles.size()-1;
	//obstacles[tr]=size;
}

void Mover::Driver::obstaclesClear()
{
	obstacles.clear();
}

void Mover::Driver::walk(const Pose & pose)
{
	target = pose.getPosition();
	targetDir = pose.getDirection();
	task.target = true;
	task.dir = true;
	updateTask();
}

void Mover::Driver::walk(const Pose::pos &t)
{
	target=t;
	task.target = true;
	task.dir = false;	
	updateTask();
}

void Mover::Driver::face(const Pose::vec &t)
{
	targetDir = t;
	task.target = false;
	task.dir = true;
	updateTask();
}

void Mover::Driver::updatePath(float dt)
{
//	LogFunction(*g_logger);
	if(pathCurrent != -1)
	{
		//_RPT0(0,"Vehicle::Driver::update()");
		if(tryNextWaypoint() && logDriver)	{};	
//			g_logger->line(1,"(next waypoint)");
		if(pathCurrent >= path.size())
		{
//			if(logDriver)
//				g_logger->line(1,"(last waypoint is reached)");	
			clearTask();
			mover->state = Idle;
			if(listener)
				listener->moverEvent(getMover(),taskSuccess);
//			else
//				g_logger->line(1,"no listener specified");	
			
			return;
		}
	}
}

bool Mover::Driver::tryNextWaypoint()
{
	if(pathCurrent!=-1)
	{
		vec2f pos = current()->v;
		float dist=vecDistance(pos,mover->getGlobalPose().getPosition());
		minDistance = mover->getMaster()->getSphereSize();
		if(dist < minDistance)
		{
			pathCurrent++;
			return true;
		}
	}
	return false;
}

const PathFinder::Path::Waypoint2 * Mover::Driver::current()	
{
	return &path[pathCurrent];
}

void Mover::Driver::clearTask()
{
//	g_logger->line(1,"task cleared");
	path = Path();
	pathCurrent =- 1;
}

int Mover::Driver::waypointsCount() const
{
	return path.size();
}

int Mover::Driver::waypoints(vec2f *points,int max) const
{
	if(max<0 || max>waypointsCount())max=waypointsCount();
	for(int i = 0; i < max; i++)
		points[i] = path[i].v;
	return max;
}
float Mover::Driver::timeToImpact()
{
	auto vel = conv(mover->getBody()->GetLinearVelocity());//mover->getDirection() * fSign(mover->velocity[0]);
	if(vel.length() < 0.00001f)
		return -1;

	Unit* unit = mover->getMaster();
	AABB2 box = mover->master->getOOBB();
	vec2f size = box.size();
	vec2f center = box.center;

	auto pose = mover->getPose();

	float distance[] = 
	{
		rayCast(pose.transformPos(center + vec2f(size[0],-size[1]/2)),vel),
		rayCast(pose.transformPos(center + vec2f(size[0],0)),vel),
		rayCast(pose.transformPos(center + vec2f(size[0], size[1]/2)),vel),
	};

	float minDistance=distance[0];
	for(int i=1;i<3;i++)
		if(distance[i]<minDistance)
			minDistance=distance[i];

	return minDistance/fabs(vel.length());	
}

float Mover::Driver::rayCast(const Pose::vec& pos,const Pose::vec& dir)const
{	
	float minDistance=std::numeric_limits<float>::max();
	Geom::_Edge<Pose::vec> ray(pos,pos+dir);
	for(int i=0;i<obstacles.size();i++)
	{
		Geom::_Sphere<Pose::vec> sphere(obstacles[i].first.getPosition(0),obstacles[i].second);
		float tmp[2];
		Pose::vec res[2];
		for(int result=intersection(ray,sphere,res,tmp);result>0;result--)
		{
			float d=tmp[result-1];
			if(d<minDistance && d>0.f)
				minDistance=d;
		}
	}
	return minDistance;
}
Pose::vec Mover::Driver::pathDirection()
{
	Pose::vec result(0.f);
	if(this->waypointsCount() && pathCurrent != -1 && pathCurrent!=waypointsCount())
	{
		//tryNextWaypoint();
		result = current()->v - getMover()->getMaster()->getPosition();
	}
	return result;
}

Pose::vec Mover::Driver::pathError()
{
	return vec2f(0,0);
}
pathProject::PathProcess * Mover::Driver::pathProcess()
{
	return getMover()->pathProcess();
}
vec3f to3(const vec2f &v)
{
	return vec3(v[0],v[1],0);
}

void Mover::Driver::updateTask()
{
//	LogFunction(*g_logger);
	clearTask();
	// simply move to that point
	if(task.target)
	{		
//		g_logger->line(2,"target=(%g,%g)",target[0],target[1]);
		if(listener)
			listener->moverEvent(getMover(),taskNew);

		pathProject::PathProcess * process = pathProcess();
		if(process && process->getCore()->getMapManager()->getMapCount())
		{
			mover->getMaster()->syncPose();
			createTask(process, mover->getGlobalPose(), target);
			waiting = true;
			mover->state = Idle;
		}
		else
		{
			pathProject::Waypoint wp;
			wp.v = to3(target);
			path.obtain(&wp,1);
			pathCurrent = 0;
			//listener->moverEvent(getMover(),taskSuccess);
		}
		/*
		int res = canReach(target, 0);
		if(!res)
		{
			g_logger->line(2,"cannot reach");
			if(listener)
				listener->moverEvent(getMover(), taskFail);
			return;
		}*/
//		g_logger->line(2,"building path");
		//if(res != 2)	// if we are in the same position
		//	buildPath();
		//else
		//	listener->moverEvent(getMover(),taskSuccess);
	}
}

void Mover::Driver::getPath(Path & path, bool success)
{
	waiting = false;
	
	if(path.size())
	{
		this->path = path;
		pathCurrent = 0;
		mover->state = Moving;
		
	}
	else if(success)
	{
		if(listener)
			listener->moverEvent(getMover(), taskSuccess);
	}
	else		
	{
		if(listener)
			listener->moverEvent(getMover(), taskFail);
		pathCurrent = -1;
	}
}

//
//int Mover::Driver::canReach(const vec2f & target, float max)
//{
//	pathProject::PathProcess * process = pathProcess();
//	if(process && process->getCore()->getMapManager()->getMapCount())
//	{		
//		Pose pose = mover->getGlobalPose();
//		process->addStart(to3(pose.getPosition()),to3(pose.getDirection()));
//		process->addTarget(to3(target));
//		pathProject::PathCore * pathCore = process->getCore();
//		pathCore->selectProcess(process);
//		return pathCore->doSearch();
//	}
//	else
//	{
//		return 1;
//	}
//}

float Mover::Driver::pathLength()
{
	float result=0.0f;
	for(int i=0;i<path.size()-1;i++)
		result+=vecDistance(path[i].v,path[i+1].v);
	return result;
}

//int Mover::Driver::buildPath()
//{
//	LogFunction(*g_logger);
//	pathProject::PathProcess * process = pathProcess();
//	mover->state = Idle;
//	int count = 0;
//	if(process && process->getCore()->getMapManager()->getMapCount())
//	{
//		count = process->getWaypointsCount();
//		if(count)
//		{
//			pathProject::Waypoint * ptr = process->getPath();
//			path.resize(0);
//			path.reserve(count);
//			Waypoint2 wp;
//
//			bool directSight = false;
//			// raycasting callback
//			auto rayCallback = [&directSight](const RayHitInfo & hit) -> float
//			{
//				directSight = true;
//				return 0;
//			};
//
//			for(int i = count - 1; i >=0; i--)
//			{
//				if( i == count - 1 || i == 0 || ptr[i].action != ptr[i-1].action)
//				{
//					wp.v = vec2f(ptr[i].v[0], ptr[i].v[1]);
//					wp.action = ptr[i].action;
//					path.push_back(wp);
//				}
//			}
//
//			//float range;
//			//Pose ray;
//
//			//mover->getManager()->objectManager->raycastGeneric(ray,range,rayCallback);
//		}
//	}
//	else
//	{
//		path.resize(1);
//		path[0].v = this->target;		
//		count = 1;
//	}
//	if(count)
//	{
//		pathCurrent = 0;
//		mover->state = Moving;	
//	}
//	return count;
//}

void Mover::Driver::update(float dt)
{
//	LogFunction(*g_logger);
	updatePath(dt);
	if(pathCurrent!=-1)
	{
		vec2f dir = path[pathCurrent].v - mover->getGlobalPose().getPosition();		
		// cos of angle between direction to target and mover direction
		// if this angle is more than M_PI_2, then we need to move backward
		mover->directionControl(normalise_s(dir), 1.0);
	}
}

void Mover::Driver::render(HGE * hge)
{
	if(pathCurrent!=-1)
	{
		vec2f lastPos = mover->getMaster()->getPosition();
		for(int i = pathCurrent; i < path.size(); i++)
		{
			drawLine(hge, lastPos, path[i].v,RGB(100,0,1));
			lastPos = path[i].v;
		}
	}
}
/*void Mover::Driver::setTarget(const vec3 &t, const vec3 &dir)
{
	target=t;
	targetDir=dir;
	task.target=true;
	task.dir=true;
	updateTask();
}*/	
////////////////////////////////////////////////////////////////////
// Mover
////////////////////////////////////////////////////////////////////

Mover::Mover(MoverDef *def)
:Device(def),state(Idle),driver(NULL),listener(NULL),lastControl(0.f)
{}
Mover::~Mover()
{
	if(driver)
		delete driver;
}

void Mover::stop()
{
	//task.clear();
	state=Idle;
}

Mover::State Mover::getState() const
{
	return state;
}

Mover::Driver * Mover::getDriver()
{
	return driver;
}

const Mover::Driver * Mover::getDriver() const
{
	return driver;
}

Pose::vec Mover::getLastControl()const
{
	return lastControl;
}

void Mover::clearDriver()
{
	if(driver)
	{
		delete driver;
		driver=NULL;
	}
}

void Mover::setDriver(Mover::Driver *d)
{
	
	clearDriver();
	driver=d;
	if(driver)
		setMode(dmDriver);
}
void Mover::updateTask()
{
	state=Moving;
	if(driver)
		driver->updateTask();
}

int Mover::writeState(IO::StreamOut &buffer)
{
	return 0;
}

int Mover::readState(IO::StreamIn &buffer)
{
	return 0;
}

pathProject::PathProcess * Mover::pathProcess()
{
	return dynamic_cast<MoverDef*>(getDefinition())->pathProcess;
}

#ifdef DEVICE_RENDER
void Mover::render(HGE * hge)
{
	if(driver)
	{
		driver->render(hge);
		/*
		vec2f size=getUnit()->getOOBB();
		vec2f start[] =
		{
			getPose().transformPos(vec2f(size[0],-size[1])),
			getPose().transformPos(vec2f(size[0],0)),
			getPose().transformPos(vec2f(size[0], size[1])),
		};
		for(int i=0;i<3;i++)
		{
			float distance=driver->rayCast(start[i],getDirection());
			
			vec2f end=start[i]+getDirection()*distance;
			drawLine(hge,start[i],end,RGB(0,0,128));
			drawPoint(hge,end,RGB(0,0,128),10);
		}*/
		
	}
}
#endif
//////////////////////////////////////////////////////////////////////
//// Mover
//////////////////////////////////////////////////////////////////////
Mover::Definition::Definition(DeviceManager &manager)
:DeviceDef(manager),pathProcess(NULL)
{
	pathProcess = manager.pathCore->createSearch();			// for general pathfinding
	adjacency = 1;
	bounds = false;
	smooth = false;
	heuristic = false;
}

Mover::Definition::~Definition()
{
	if(pathProcess)
		pathProcess->release();
}

}
