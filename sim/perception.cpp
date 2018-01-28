#include "perception.h"

#include "device.h"
#include "inventory.h"
#include "moverVehicle.h"
#include "objectManager.h"
#include "vision.h"
#include "projectile.h"
#include "unit.h"

namespace sim
{
////////////////////////////////////////////////////////
// PerceptionClient
////////////////////////////////////////////////////////
PerceptionClient::PerceptionClient(int max)
{
	objects.reserve(max);
	memset(&query,0,sizeof(query));
}

#ifdef FUCK_THIS
PerceptionDef::PerceptionDef(ObjectManager & m)
:DeviceDef(m)
{}

PerceptionDef::~PerceptionDef()
{}

Device * PerceptionDef::create(StreamIn *context)
{
	return new Perception(this);
}
#endif
////////////////////////////////////////////////////////
// Perception
////////////////////////////////////////////////////////
Perception::Perception(Perception* def)
:Device(def)
{
	unit = nullptr;
	client = nullptr;
	active = true;
	if (def != nullptr)
	{
		fov = def->fov;
		distance = def->distance;
	}
}

Perception::~Perception()
{}

void Perception::update(float dt)
{
	if(active)
	{
		GameObject* owner = master;
		if(owner != nullptr)
		{
			auto manager = owner->getManager();
			if(manager)
			{
				manager->getVisionManager()->addVision( distance, fov, getGlobalPose(), owner );
			}
		}
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

void Perception::getProjectiles(Perception::Objects& objects)
{
	for(Observed::iterator it=observed.begin();it!=observed.end();++it)
	{
		Projectile * obj=dynamic_cast<Projectile *>(*it);
		if(obj && isHostile(obj,unit))		
			objects.push_back(obj);
	}
}
void Perception::getHostile(Perception::Units& objects)
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
int Perception::writeState(StreamOut& buf){return 0;}

int Perception::readState(StreamIn& buf){return 0;}

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
		if(i && i->state==ItemStatus::OnGround)
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
// TODO: implement this class
////////////////////////////////////////////////////
class ImpactImage
{
public:
	std::vector<float> map;
	int size;
	float cellWidth;
	float cellHeight;
	Pose pose;
	using pos = Pose::pos;

	/**
	 * ImactImage constructor
	 * @param w - cell width
	 * @param h - cell height
	 * @param size grade. Real size will be 2^size
	 */
	ImpactImage(float w, float h, int size)
	{
		this->size = size;
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

	void setPose(const Pose& p)
	{
		//pose=Mt4x4::translate(
	}

	void clear()
	{
		map.assign(width()*height(),0);
	}

	pos world2map(const pos &v)
	{
		return pose.projectPos(v);
	}

	void registerObject(const pos& position,const pos& velocity)
	{
		pos start=world2map(position);
		// TODO: implement it
	}
};


// get movement trajectory from GameObject
Trajectory2 getTraectory2(GameObjectPtr object)
{
	vec2f position = vec2f::zero();
	vec2f velocity = vec2f::zero();

	if(object)
	{
		position = object->getPosition();
		velocity = conv(object->getBody()->GetLinearVelocity());
	}
	/*
	// What's the heck is that?
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
	}*/
	return Trajectory2(position, velocity);
}

}
