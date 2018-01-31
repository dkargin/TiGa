#pragma once
#include "device.h"
#include "objectManager.h"

namespace sim
{

class Unit;
class Weapon;

struct WeaponClient
{
public:
	virtual ~WeaponClient() {}
	virtual void onInstallWeapon(Weapon * object,int mount)=0;
};

struct WeaponData
{
	float timeShootDelay = 1.0;			//< Delay between shoots
	float timeShootDuration = 1.0f;	//< Duration of 'shoot' phase
	float timeReload;								//< Reload time
	//< Offset from the muzzle. Used as spawning point for projectiles
	float muzzleOffset;
	int maxAmmo = 1;
	float maxRange = 1000;			// max range
	float spread = 0.03;			// projectile spread, degrees
	int barrels = 1;
	float barrelDistance;	//< distance between barrels
	int animations = 1;		//< number of animations
	int burstSize = 1;			//< Number of shots done
	float burstDelay = 0;		//< Delay between the bursts
};

class Weapon: public Device
{
public:
	WeaponData weaponData;
	float time;
	int ammo;
	bool fire;					//< Are we firing now?
	int burstLeft;			//< Number of bursts left before reload

	Fx::EntityPtr fxShoot;

	enum {portShoot=0};
	enum WeaponState
	{
		stateReady,
		stateBurst,
		stateDelay,
		stateReload
	}weaponState;

	Weapon(Weapon* def, StreamIn *context = NULL);
	~Weapon();

	float getEffectiveRange() const;
	int writeState(StreamOut& buffer);
	int readState(StreamIn& buffer);
	virtual void updateBurst(float dt);
	virtual void shoot();
	
	virtual void update(float dt);
	bool validCommand(int port, DeviceCmd cmd)const;
	int execute_Action(int port);
	virtual Pose getMuzzlePose();	// get world pose	
private:
	virtual void doShoot(float dt);
};

#ifdef FUCK_THIS
class WeaponDef: public DeviceDef
{
public:
	WeaponData weaponData;
	// ��������� �������������� ������
	ProjectileDef *projectile;

	FxPointer fxShoot;	// effects for idle state

	float getEffectiveRange()const;
public:
	WeaponDef(DeviceManager &m);
	~WeaponDef();
	virtual Device * create(IO::StreamIn *context = NULL);
};
#endif

} // namespace sim
