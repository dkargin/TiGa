#pragma once
#include "objectManager.h"
#include "device.h"
class ProjectileDef;
class Unit;

class Weapon;
class WeaponDef;

struct WeaponClient
{
public:
	virtual void onInstallWeapon(Weapon * object,int mount)=0;
};

class Weapon: public Device
{
	WeaponDef * definition;
public:	
	float time;				// текущий счётчик	
	int ammo;
	bool fire;				
	int burstLeft;			// оставшееся количество выстрелов в очереди

	FxEffect *fxShoot;

	enum {portShoot=0};
	enum WeaponState {stateReady,stateBurst,stateDelay,stateReload}weaponState;
	Weapon(WeaponDef * def,IO::StreamIn *context = NULL);
	~Weapon();

	float getEffectiveRange()const;
	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);	
	virtual void updateBurst(float dt);
	virtual void shoot();
	
	virtual void update(float dt);
	bool validCommand(int port,DeviceCmd cmd)const;	
	int execute_Action(int port);
	virtual Pose getMuzzlePose();	// get world pose	
	virtual DeviceDef * getDefinition();
private:
	virtual void doShoot(float dt);
};

class WeaponDef: public DeviceDef
{
public:
	float timeShootDelay;	// задержка между выстрелами
	float timeShootDuration;// длительность выстрела, для лучевых оружий
	float timeReload;		// время перезарядки оружия
	float muzzleOffset;		// откуда создаётся снаряд
	int maxAmmo;			
	float maxRange;			// max range
	float spread;			// projectile spread, degrees
	int barrels;			
	float barrelDistance;	// distance between barrels
	int animations;			// number of animations	
	int burstSize;			// количество снарядов в одном залпе
	float burstDelay;		// задержка между выстрелами в залпе
	// параметры прожектильного оружия
	ProjectileDef *projectile;

	FxPointer fxShoot;	// effects for idle state

	float getEffectiveRange()const;
public:
	WeaponDef(DeviceManager &m);
	~WeaponDef();
	virtual Device * create(IO::StreamIn *context = NULL);
};