#include "stdafx.h"
#include "objectManager.h"
#include "device.h"
#include "unit.h"
#include "moverVehicle.h"
#include "inventory.h"
#include "predictor.h"
#include "perception.h"
#include "projectile.h"

////////////////////////////////////////////////////////
// PerceptionClient
////////////////////////////////////////////////////////
PerceptionClient::PerceptionClient(int max)
{
	objects.reserve(max);
	memset(&query,0,sizeof(query));
}
////////////////////////////////////////////////////////
// PerceptionDef
////////////////////////////////////////////////////////
PerceptionDef::PerceptionDef(ObjectManager & m)
:DeviceDef(m)
{}

PerceptionDef::~PerceptionDef()
{}

Device * PerceptionDef::create(IO::StreamIn *context)
{
	return new Perception(this);
}
////////////////////////////////////////////////////////
// Perception
////////////////////////////////////////////////////////
Perception::Perception(PerceptionDef * def)
:Device(def), definition(def), unit(NULL), client(NULL), active(true)
{}

Perception::~Perception()
{}

void Perception::update(float dt)
{
	if(active)
	{
		GameObject * owner = this->master;
		getManager()->addVision( definition->distance, definition->fov, getGlobalPose(), owner );
		//((PerceptionManager&)definition->manager).updatePerception(this,dt);
	}
}

Pose Perception::getGlobalPose() const
{
	if(master)
		return Device::getGlobalPose();
	else if(unit)
		return unit->getPose();
	return Pose(0,0);
}

void Perception::getFriendly(Perception::Units &units)
{
	for(Observed::iterator it=observed.begin();it!=observed.end();++it)
	{
		Unit *u=dynamic_cast<Unit *>(*it);
		if(u && unit->player==u->player)		
			units.push_back(u);
	}
}

bool isHostile(const GameObject *a,const GameObject *b)
{
	int pa = a->getPlayer();
	int pb = b->getPlayer();
	if( pa == 0 || pb == 0)
		return false;
	return (a && b)?(pa != pb) : false;
}

void Perception::getProjectiles(Perception::Objects &objects)
{
	for(Observed::iterator it=observed.begin();it!=observed.end();++it)
	{
		Projectile * obj=dynamic_cast<Projectile *>(*it);
		if(obj && isHostile(obj,unit))		
			objects.push_back(obj);
	}
}
void Perception::getHostile(Perception::Units &objects)
{
	for(Observed::iterator it=observed.begin();it!=observed.end();++it)
	{
		Unit *obj=dynamic_cast<Unit *>(*it);
		if(obj && isHostile(obj,unit))		
			objects.push_back(obj);
	}
}

void Perception::onInstall(Unit * unit, size_t id, const Pose & pose)
{
	this->unit = unit;
	Device::onInstall(unit,id,pose);
}
int Perception::writeState(IO::StreamOut &buf){return 0;}

int Perception::readState(IO::StreamIn &buf){return 0;}

void Perception::removeDead()
{
	for(Observed::iterator it=observed.begin();it!=observed.end();)
	{
		GameObject *object=*it;
		if(object->isDead())
			it=observed.erase(it);
		else
			it++;
	}
}
void Perception::getItems(Perception::Items &items)
{
	for(Observed::iterator it=observed.begin();it!=observed.end();++it)
	{
		Item *i=dynamic_cast<Item*>(*it);
		if(i && i->state==Item::placeLand)
			items.push_back(i);
	}
}

bool Perception::validCommand(int port,DeviceCmd cmd)const
{
	return false;
}

//int Perception::onInstall(Assembly * owner,int mount)
//{
//	unit = owner->getUnit();
//	return Device::onInstall(owner,mount);
//}

////////////////////////////////////////////////////////
// PerceptionManager
////////////////////////////////////////////////////////
//PerceptionManager::PerceptionManager(ObjectManager *um)
//	:DeviceManager(um)
//{}
//PerceptionManager::~PerceptionManager()
//{}
//
//void PerceptionManager::release()
//{
//	delete this;
//}
//
//void PerceptionManager::update(float dt)
//{}
//
//void PerceptionManager::updatePerception(Perception *p,float dt)
//{	
//	PerceptionClient * client=NULL;
//	if(!p || !p->unit || !(client = p->unit->getPerceptionClient()))
//		return;
//	p->observed.clear();
//	client->reset();
//	
//	if(objectManager)	// search through all units visible by perception <p>
//	{
//		ObjectManager *manager=objectManager;
//		for(auto it = manager->Objects::begin();
//			it != manager->Objects::end();
//			it++)
//		{
//			GameObject *obj=(GameObject*)it->second;
//			if(obj->isDead() || p->unit==dynamic_cast<Unit*>(obj)) continue;		// ignore self
//			if(canSee(p,obj))		
//			{
//				client->addObject(obj);//observed.insert(obj);		
//			}
//		}
//	}
//}
//

////////////////////////////////////////////////////
// ImpactImage - raster image around object, where 
//(x,y) shows time, when it would be occupied
////////////////////////////////////////////////////

class ImpactImage
{
public:
	std::vector<float> map;
	int size;
	float cellWidth;
	float cellHeight;
	Mt4x4 pose;

	ImpactImage(float w,float h,int s)
		:pose(Mt4x4::identity())
	{
		size=s;
		//width=w;
		//height=h;
		clear();
	}
	int width()
	{
		return 2<<size;
	}
	int height()
	{
		return 2<<size;
	}
	void setPose(const Mt4x4 &p)
	{
		//pose=Mt4x4::translate(
	}
	void clear()
	{
		map.assign(width()*height(),0);
	}
	vec3 project(const vec3 &v)
	{
		return pose.project(v);
	}
	void registerObject(const vec3 &pos,const vec3 &vel)
	{
		vec3 start=project(pos);
	}
};

///////////////////////////////////////////////////////////////////////
// Utilities
///////////////////////////////////////////////////////////////////////
// get movement traectory from GameObject
Geom::Traectory2 getTraectory2(GameObject * object)
{
	GameObject * s=dynamic_cast<GameObject *>(object);
	vec2f position = vec2f::zero();
	vec2f velocity = vec2f::zero();
	if(s)
	{
		position = object->getPosition();
		velocity = conv(s->getBody()->GetLinearVelocity());	
	}
	else
	{
		Unit * unit=dynamic_cast<Unit *>(object);
		if(unit)
		{
			position = object->getPosition();
			velocity = conv(unit->getBody()->GetLinearVelocity());			
		}
		else
			position = object->getPosition();	
	}
	return Geom::Traectory2(position, velocity);
}