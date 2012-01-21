#pragma once
#include "gameObject.h"
#include "objectManager.h"
#include "device.h"
class Unit;
class Item;
class Inventory;
class Weapon;
class WeaponDef;
class ObjectManager;
class Mover;
class Perception;
class PerceptionClient;
class PerceptionDef;
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

//////////////////////////////////////////////////////////////
//base class for all units with devices
//////////////////////////////////////////////////////////////
class UnitDef: public GameObjectDef//,public AssemblyDef
{
public:
	friend class ObjectManager;
	friend class Unit;

	OBJECT_DEF(Unit,typeUnit);

	float health;

	FxEffect *fxMove;
	std::vector<MountDef> mounts;
	//enum{MaxMounts = 16};
	
	//StaticArray<MountDef,MaxMounts> mounts;
//	/// adds mounta
public:	
	UnitDef(ObjectManager *_manager) ;
	virtual ~UnitDef();

	size_t addMount(const MountDef & mount);
	void clearMounts();
	
	int init();
	Unit* construct(IO::StreamIn *context);
	GameObject * create(IO::StreamIn * buffer);
protected:
	UnitDef(const UnitDef& def);
};

class Unit: public GameObject
{
	friend class Device;
public:
	struct DeviceListener : virtual public Referenced
	{
		virtual void onInstallDevice(Device::Pointer device, int id) {}
		virtual void onRemoveDevice(Device::Pointer device) {}
		//virtual void onInitFinished(Assembly * assembly) {}
	};
	typedef ObjectManager Manager;

	//Inventory *inventory;
	//CommandAI * ai;
	bool local;
	bool useAI;
	unsigned char  stats[StatsMax];

	std::vector<Device*> devices;
	//Mover *mover;
	//Perception * perception;

	Controller *controller;			// player or AI
	UnitDef *definition;
public:		
	OBJECT_IMPL(Unit,typeUnit);
	Unit(UnitDef *def);	
	~Unit();
	
	Controller * getController();
	void setController(Controller * controller);
	void enableAI(bool flag);
	
	inline b2World * getDynamics()
	{
		return getManager()->scene;
	}	

	bool toSync()const;
	void toSync(bool flag);
	
	PerceptionClient * getPerceptionClient();
	//WeaponClient * getWeaponClient();
	void updateEvents();	// react to world events - update all tasks
	virtual void startAnimation(const char *name);
	virtual void stopAnimation(const char *name);
	virtual void update(float dt);	

	/// from Assembly
	virtual void useDevice(int device,int port,int action,IOBuffer *buffer);		
	// NetObject implementation
	//int writeDesc(IOBuffer &buffer);
	//int readDesc(IOBuffer &buffer);
	Device * getDevice(size_t id);

	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);	
	// for level saving
	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);
	// commands
	void cmdStop();	// clear all orders
	void cmdStand(float distance);
	void cmdMoveTo(float x,float y,float distance);
	void cmdFollow(Unit * target);
	void cmdPatrol(float x,float y,float distance);
	void cmdAttack(GameObject* target);	
	void cmdGuard(float x,float y,float distance);
};

typedef SharedPtr<Unit> UnitPtr;

/////////////////////////////////////////////////////////////////
// lua/tolua utilities
bool equal(const GameObject * a,const GameObject * b);
Unit *toUnit(GameObject *object);
