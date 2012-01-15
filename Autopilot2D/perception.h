#pragma once;
#include "objectManager.h"
#include "device.h"
#include "gameObject.h"

class Unit;
class ObjectManager;
class Item;
class Perception;
class PerceptionDef;

class PerceptionLaser;
class PerceptionLaserDef;

class Sensor
{
public:
	b2Body * body;
	Sensor(float size);
	~Sensor();
	void setSize();
	float getSize();
};


//////////////////////////////////////////////////////////////////
// to be replaced in DeviceManager
//class PerceptionManager: public DeviceManager
//{
//public:
//	PerceptionManager(ObjectManager *um);
//	virtual ~PerceptionManager();	
//	virtual void release();
//	virtual void update(float dt);	
//	virtual void updatePerception(Perception * p,float dt);
//	virtual bool canSee(Perception *p,GameObject * object);
//	//Perception *perception(const Objects::iterator &it);
////protected:
//};


/////////////////////////////////////////////////////////////////
//// to reimplement using PhysX sensors
class PerceptionDef: public DeviceDef
{
	friend class Perception;
	friend class PerceptionManager;
public:	
	float distance;		// view distance
	float fov;			// field of view in degrees

	PerceptionDef(ObjectManager & manager);
	virtual ~PerceptionDef();
	Device *create(IO::StreamIn *context=NULL);
};

class PerceptionClient
{
public:
	typedef std::vector<GameObjectPtr> Objects;
	typedef std::list<Unit*> Units;
	typedef std::list<Item *> Items;
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
		if(object != NULL || !onNotice(object))
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
	typedef std::set<GameObject *> Observed;
	Observed observed;
	GameObject * unit;
	PerceptionClient * client;
	PerceptionDef * definition;

	bool active;	// if device recieves any updates

	Perception(PerceptionDef *def);

	virtual ~Perception();

	virtual DeviceDef * getDefinition()
	{
		return definition;
	}
	virtual void onInstall(Unit * unit, size_t id, const Pose & pose);
	virtual void getFriendly(Perception::Units &units);
	virtual void getHostile(Perception::Units &units);
	virtual void getProjectiles(Objects &objects);

	virtual Pose getGlobalPose() const;

	virtual void getItems(Items &items);
	virtual void removeDead();

	//virtual int init(Unit *owner,int id,int parent=-1);
	virtual void update(float dt);
	// sync
	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);
	int writeDesc(IOBuffer &buffer);
	int readDesc(IOBuffer &buffer);

	bool validCommand(int port,DeviceCmd cmd)const;
};


//typedef std::map<float,std::pair<vec3,GameObject *> > CollisionMap;
Geom::Traectory2 getTraectory2(GameObject * object);
//int processCollisions(Perception *perception,CollisionMap & collisionMap);
