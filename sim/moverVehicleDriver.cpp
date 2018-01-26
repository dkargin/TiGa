#include "device.h"
#include "mover.h"
#include "moverVehicle.h"
#include "unit.h"

using namespace pathProject;
using namespace std;

extern Log * g_logger;
static bool logDriver=true;
static int driverMsgLevel=1;
/////////////////////////////////////////////////////////////////////////
// DriverFlocking
/////////////////////////////////////////////////////////////////////////
DriverFlocking::DriverFlocking(Mover* mover)
:Driver(mover)
{}
Pose::vec DriverFlocking::forceLeader()
{
	return Pose::vec(0.f);
}
Pose::vec DriverFlocking::forcePath()
{
	Pose::vec result(0.f);
	if(waypointsCount())
	{
		//tryNextWaypoint();
		result = normalise(current()->v-mover->getPosition());
	}
	return result;
}

Pose::vec DriverFlocking::forceObstacles()
{
	typedef Geom::Traectory2 Traectory2;
	pairs.clear();
	const float collisionDistance=30;
	const float kTime=0.05f;
	Pose::vec result(0.f);
	Traectory2 tr0(mover->getPosition(),conv(mover->getBody()->GetLinearVelocity()));
	for(Obstacles::iterator it=obstacles.begin();it!=obstacles.end();++it)
	{
		Pose::vec hits[2];
		float time=0;
		const Traectory2 &tr1=it->first;
		getMinDistance(tr0,tr1,20,time);		
		hits[0]=tr0.getPosition(time);
		hits[1]=tr1.getPosition(time);

		pairs.push_back(std::make_pair(hits[0],hits[1]));
		float distance = vecDistance(hits[0],hits[1]);
		if(distance < collisionDistance)
		{
			Traectory2::vec delta = distance > 0.f ? hits[0]-hits[1] : tr0.getPosition(0) - tr1.getPosition(0);
			Pose::vec force = normalise(delta)*kTime/(kTime+time);
			result += force;
		}
	}
	return result;
}

void DriverFlocking::update(float dt)
{
	updatePath(dt);
	const float kObstacles=30.f;
	const float kLeader=1.f;
	const float kPath=0.2f;
	Pose::vec sumForce=Pose::vec(0.f);
	
	fLeader=forceLeader();
	fObstacles=forceObstacles();
	fPath=forcePath();
	sumForce=	fLeader*kLeader+
				fObstacles*kObstacles+
				fPath*kPath;
	
	mover->directionControl(sumForce, sumForce.length());
}

void DriverFlocking::render(HGE * hge)
{
	Mover::Driver::render(hge);
	Pose::pos p=mover->getPosition();
	float forceScale=30;
	drawArrow(hge,p,p+forceScale*fLeader,RGB(255,0,0));
	drawArrow(hge,p,p+forceScale*fPath,RGB(0,255,0));
	drawArrow(hge,p,p+forceScale*fObstacles,RGB(255,0,255));

	for(auto it = pairs.begin();it!=pairs.end();++it)
	{
		auto &tr1=it->first;
		drawPoint(hge,tr1,RGB(0,255,0),10);
	}
	for(auto it = obstacles.begin();it!=obstacles.end();++it)
	{

		
	}	
}

Mover::Driver * createFlockingDriver(Mover *m)
{
	return new DriverFlocking(m);
}
