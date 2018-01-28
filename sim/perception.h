#pragma once

#include "objectManager.h"
#include "device.h"
#include "gameObject.h"

// Here are classes, that imitate some perception sensors from robots,
// like lidars or cameras

namespace sim
{

class Unit;
class ObjectManager;
class Item;
class Perception;
class PerceptionLaser;

// WTF is this?
class Sensor
{
public:
	b2Body* body;

	Sensor(float size);
	~Sensor();

	void setSize();
	float getSize();
};

#ifdef FUCK_THIS
class PerceptionDef: public DeviceDef
{
	friend class Perception;
	friend class PerceptionManager;
public:	

	PerceptionDef(ObjectManager & manager);
	virtual ~PerceptionDef();
	Device *create(StreamIn* context=NULL);
};
#endif

class PerceptionClient
{
public:
	typedef std::vector<GameObjectPtr> Objects;
	//typedef std::list<Unit*> Units;
	//typedef std::list<Item *> Items;
	typedef std::set<GameObjectPtr> Observed;
	
	struct				/// do we notice ...	
	{
		int units:1;	/// ... any units
		int enemy:1;	/// ... enemy objects
		int allied:1;	/// ... allied objects
		int projectiles:1;
	}query;

	PerceptionClient(int maxObjects=16);
	virtual ~PerceptionClient(){}
	virtual bool onNotice(GameObjectPtr object)
	{
		return true;
	}
	virtual bool addObject(GameObjectPtr object)
	{
		if(object != nullptr || !onNotice(object))
			return false;
		objects.push_back(object);
		return true;
	}
	virtual void reset()
	{
		objects.clear();
	}
protected:
	Objects objects;
};

class Perception: public Device
{
public:
	typedef std::list<Unit*> Units;
	typedef std::list<Item *> Items;
	typedef std::list<GameObject * > Objects;
	typedef std::set<GameObject*> Observed;

	Observed observed;
	GameObject* unit;
	PerceptionClient* client;

	float distance;		//< view distance
	float fov;				//< field of view in degrees
	bool active;			//< if device recieves any updates

	Perception(Perception *def=nullptr);
	virtual ~Perception();

	virtual void onInstall(Unit * unit, size_t id, const Pose & pose);
	virtual void getFriendly(Perception::Units &units);
	virtual void getHostile(Perception::Units &units);
	virtual void getProjectiles(Objects &objects);

	virtual Pose getGlobalPose() const;

	virtual void getItems(Items &items);
	virtual void removeDead();

	virtual void update(float dt);
	// sync
	int writeState(StreamOut &buffer);
	int readState(StreamIn &buffer);
	int writeDesc(IOBuffer &buffer);
	int readDesc(IOBuffer &buffer);

	bool validCommand(int port,DeviceCmd cmd)const;
};

// Fill in linear trajectory for game object
Trajectory2 getTraectory2(GameObjectPtr object);

} // namespace sim
