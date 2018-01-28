#include "device.h"
#include "mover.h"
#include "moverVehicle.h"
#include "unit.h"

namespace sim
{

MoverFlockingAI::MoverFlockingAI(Mover* mover)
:Driver(mover)
{}

Pose::vec MoverFlockingAI::forceLeader()
{
	return Pose::vec(0.f);
}

Pose::vec MoverFlockingAI::forcePath()
{
	Pose::vec result(0.f);

	if(getWaypointsCount() > 0)
	{
		//tryNextWaypoint();
		Pose pose = getCurrentWaypoint();
		result = (pose.getPosition() - mover->getPosition()).normalize();
	}
	return result;
}

Pose::vec MoverFlockingAI::forceObstacles()
{
	pairs.clear();
	const float collisionDistance=30;
	const float kTime=0.05f;
	Pose::vec result(0.f);

	Trajectory2 tr0(mover->getPosition(),conv(mover->getBody()->GetLinearVelocity()));
	for(Obstacles::iterator it=obstacles.begin();it!=obstacles.end();++it)
	{
		Pose::vec hits[2];
		float time = 0;
		const Trajectory2& tr1 = it->first;
		math3::geometry::getMinDistance(tr0,tr1,20,time);
		hits[0]=tr0.getPosition(time);
		hits[1]=tr1.getPosition(time);

		pairs.push_back(std::make_pair(hits[0],hits[1]));
		float distance = vecDistance(hits[0],hits[1]);
		if(distance < collisionDistance)
		{
			vec2f delta = distance > 0.f ? hits[0] - hits[1] : tr0.getPosition(0) - tr1.getPosition(0);
			vec2f force = vec2f::normalize_s(delta)*kTime/(kTime+time);
			result += force;
		}
	}
	return result;
}

void MoverFlockingAI::update(float dt)
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

void MoverFlockingAI::render(Fx::RenderContext* rc)
{
	Mover::Driver::render(rc);
	Pose::pos p = mover->getPosition();
	float forceScale=30;
	drawArrow(rc, p, p + forceScale*fLeader, Fx::MakeRGB(255,0,0));
	drawArrow(rc, p, p + forceScale*fPath, Fx::MakeRGB(0,255,0));
	drawArrow(rc, p, p + forceScale*fObstacles, Fx::MakeRGB(255,0,255));

	for(const auto& tr1: pairs)
	{
		drawPoint(rc, tr1.first, Fx::MakeRGB(0,255,0), 10);
	}

	for(auto it = obstacles.begin();it!=obstacles.end();++it)
	{

		
	}	
}

Mover::Driver * createFlockingDriver(Mover *m)
{
	return new MoverFlockingAI(m);
}

}
