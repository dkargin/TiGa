#pragma once

#include "device.h"
#include "gameObject.h"
#include "objectManager.h"

namespace sim
{

class Unit;
class Item;
class Device;
class Inventory;
class Weapon;
class ObjectManager;
class Mover;
class Perception;
class PerceptionClient;
class PerceptionManager;
class Controller;

enum Stats
{
	Food,
	Energy,
	Comfort,
	Interest,
	Hitpoints,
	StatsMax
};

enum UnitState
{
	usWait,
	usMove
};

/// Description for mounting point
/// Device can be attached to mount points
struct MountDef
{
	Pose pose;
	Device* device = nullptr;
};

//////////////////////////////////////////////////////////////
//base class for all units with devices
//////////////////////////////////////////////////////////////
#ifdef FUCK_THIS
class UnitDef: public GameObjectDef
{
public:
	friend class ObjectManager;
	friend class Unit;

	//OBJECT_DEF(Unit, typeUnit);

	float health;

	Fx::EntityPtr fxMove;
	std::vector<MountDef> mounts;
	
	//StaticArray<MountDef,MaxMounts> mounts;
//	/// adds mounta
public:	
	UnitDef(ObjectManager *_manager) ;
	virtual ~UnitDef();

	size_t addMount(const MountDef & mount);
	void clearMounts();
	
	int init();
	Unit* construct(StreamIn *context);
	GameObject * create(StreamIn * buffer);
protected:
	UnitDef(const UnitDef& def);
};
#endif

class Unit: public GameObject
{
	friend class Device;
public:

	class DeviceListener
	{
	public:
		virtual void onInstallDevice(std::shared_ptr<Device> device, int id) {}
		virtual void onRemoveDevice(std::shared_ptr<Device> device) {}
		//virtual void onInitFinished(Assembly * assembly) {}
	};

	typedef ObjectManager Manager;

	bool local;
	bool useAI;
	unsigned char  stats[StatsMax];

	std::vector<Device*> devices;
	std::unique_ptr<Controller> controller;			// player or AI
	Unit *definition;

	Fx::EntityPtr fxMove;
	std::vector<MountDef> mounts;

public:		
	Unit(Unit* def=nullptr);
	~Unit();
	
	Controller * getController();
	void setController(Controller * controller);
	void enableAI(bool flag);
	
	b2World * getDynamics();

	bool toSync()const;
	void toSync(bool flag);
	
	PerceptionClient * getPerceptionClient();
	//WeaponClient * getWeaponClient();
	void updateEvents();	// react to world events - update all tasks
	virtual void startAnimation(const char *name);
	virtual void stopAnimation(const char *name);
	virtual void update(float dt);	

	/// from Assembly
	virtual void useDevice(int device,int port,int action, IOBuffer *buffer);
	// NetObject implementation
	//int writeDesc(IOBuffer &buffer);
	//int readDesc(IOBuffer &buffer);
	Device * getDevice(size_t id);

	int writeState(StreamOut &buffer);
	int readState(StreamIn &buffer);
	// for level saving
	virtual void save(StreamOut & stream);
	virtual void load(StreamIn & stream);
	// commands
	void cmdStop();	// clear all orders
	void cmdStand(float distance);
	void cmdMoveTo(float x,float y,float distance);
	void cmdFollow(Unit * target);
	void cmdPatrol(float x,float y,float distance);
	void cmdAttack(GameObjectPtr target);
	void cmdGuard(float x,float y,float distance);
};

typedef std::shared_ptr<Unit> UnitPtr;

/////////////////////////////////////////////////////////////////
// lua/tolua utilities
bool equal(const GameObject * a,const GameObject * b);
Unit *toUnit(GameObject *object);

} // namespace sim
