#include "stdafx.h"
//#include "world.h"
#include "device.h"
#include "assembly.h"
#include "weapon.h"
#include "weaponBeam.h"
#include "unit.h"
#include "projectile.h"

WeaponBeam::WeaponBeam(WeaponBeamDef *def,Device::BuildContext *context)
	:Object(def,context),fire(false)
{
	if(def->beam)
	{
		beam=new FxBeam(*def->beam);
		beam->visible=false;
		effects.attach(beam);
	}
}

WeaponBeam::~WeaponBeam()
{
	if(beam)
		delete beam;
}

struct RayHitInfo
{
	GameObject *object;
	vec2f point,normal;
	float fraction;
	float distance;
	static bool compare(const RayHitInfo &a,const RayHitInfo &b)
	{
		return a.distance<b.distance;
	}
};
class Raycaster: public b2RayCastCallback
{
	b2World *world;
public:	
	Raycaster(b2World *w):world(w)
	{
		hits.reserve(255);
	}
	typedef std::vector<RayHitInfo> Hits;
	Hits hits;
	Pose ray;
	Hits &cast(const Pose &r,float range)
	{
		ray=r;
		world->RayCast(this,b2conv(ray.position),b2conv(ray.position+range*ray.getDirection()));
		std::sort(hits.begin(),hits.end(),RayHitInfo::compare);
		return hits;
	}
	virtual float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction)
	{
		RayHitInfo hit;
		hit.point=conv(point);
		hit.normal=conv(normal);
		hit.object=(GameObject*)fixture->GetUserData();
		hit.fraction=fraction;
		hit.distance=vecDistance(ray.position,hit.point);
		hits.push_back(hit);
		return 1;
	}
};
void WeaponBeam::update(float dt)
{
	Weapon::update(dt);
	if(fire)
	{
		if(time<=0)
		{
			beam->stop();
			beam->visible=0;
			time=definition->delay;
			fire=false;
		}
		else
		{
			b2World *world=definition->manager->objectManager->scene;
			Raycaster raycaster(world);
			float length=definition->range;
			raycaster.cast(getMuzzlePose(),length);
			//std::vector<RayHitInfo> hits;

			Pose pose=getMuzzlePose();
			//rayCast(world,hits,pose.getPosition(),pose.getDirection());
			
			if(!raycaster.hits.empty())
				length=vecLength(pose.position-raycaster.hits.front().point);
			beam->setLength(length);
		}
	}
}
int WeaponBeam::execute_Action(int port)
{
	if(port!=portShoot)return 0;
	
	if(time<=0 && !fire)
	{
		if(fxShoot)	fxShoot->start();	
		//b2World *world=definition->manager->objectManager->scene;
		//std::vector<RayHitInfo> hits;
		//Pose pose=getMuzzlePose();
		//..rayCast(world,hits,pose.getPosition(),pose.getDirection());
		
		beam->visible=true;
		beam->setPose(Pose(definition->muzzleOffset,0,0));
		beam->setLength(definition->range);
		beam->start();

		fire=true;
		/*if(definition->projectile)
		{
			GameObject *object=definition->projectile->create(NULL);
			if(object)				
				object->setPose(getMuzzlePose());					
		}	*/	
		time=definition->beamLife;
	}
	return 1;	
}
int WeaponBeam::writeSync(IOBuffer &buffer)
{
	return buffer.write(time);
}
int WeaponBeam::readSync(IOBuffer &buffer)
{
	return buffer.read(time);
}
