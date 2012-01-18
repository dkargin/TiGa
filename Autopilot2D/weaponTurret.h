#pragma once
#include "weapon.h"
#include "unit.h"
class ProjectileDef;
class Unit;

class WeaponTurret;
class WeaponTurretDef;
//typedef DevicePair<WeaponTurretDef,WeaponTurret,WeaponPair> WeaponTurretPair;

class TurretDriver;
class WeaponTurret: public Weapon//, virtual public ProtoImpl<WeaponTurret,Device>
{
	friend class TurretDriver;	
	float angle;		// angle in degrees
	float direction;	// -1,0,1
	
	WeaponTurretDef * definition;
public:	
	enum {portTurn=1};

	WeaponTurret(WeaponTurretDef * definition);
	//WeaponTurret(DeviceManager *manager);
	~WeaponTurret();

	float getDimensions() const ;
	void stop();
	//int writeDesc(IOBuffer &context);
	//int readDesc(IOBuffer &context);
	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);

	virtual void update(float dt);	
	virtual float getCurrentAngle() const;					// get current muzzle angle
	virtual float getAngleTo(const Pose::pos &v) const;	// get angle to a given vector
	virtual Pose getMuzzlePose();
	bool canReach(const Pose::pos &v) const;				// if vector is in reachable zone
	bool fullTurn()const;							// if region [-180,180] is reachable
	
	DeviceDef * getDefinition();

	bool validCommand(int port,DeviceCmd cmd)const;
	int execute_Direction(int port,DeviceCmd action,float value);	
};

class WeaponTurretDef: public WeaponDef
{
public:
	float velocity;
	float dimensions;
	
	WeaponTurretDef(DeviceManager &m):WeaponDef(m)
	{
		velocity=100.0f;
		dimensions=360.0f;
		invokerParam(1,HGEK_E,HGEK_Q);
	}
	Device * create(IO::StreamIn *context=NULL)
	{
		return new WeaponTurret(this);
	}
};
// lua helper functions
WeaponTurret* toTurret(Device * weapon);
//WeaponTurret::Driver * getTurretDriver(Weapon * weapon);
//
class TurretDriverDef;
//

struct TrackingInfo
{
	GameObjectPtr object;
	Pose::pos pos;	// target position
	Pose::vec vel;	// target velocity
	float size;		// target size
};

class TurretDriver//: public Device
{
	float minError;
public:
	TurretDriver();
	~TurretDriver();
	virtual bool aim(WeaponTurret * weapon,const TrackingInfo& info,float dt);
};

class TargetingSystemDef;
/// gets cmdTarget
class TargetingSystem : public Device, public Unit::DeviceListener
{	
public:
	enum { portTarget = 0 };	

	TargetingSystem(TargetingSystemDef * def);
	~TargetingSystem();

	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);

	void onInstall(Unit * unit,size_t id, const Pose & pose);

	DeviceDef * getDefinition();
	bool validCommand(int port,DeviceCmd cmd)const;
	virtual bool canControl(Device * device) const;
	virtual int execute_Target(int port,DeviceCmd subtype,const Pose::pos &target);

	virtual void onInstallDevice(Device::Pointer device, int id);

	virtual void checkDevices();
protected:
	virtual bool executeControl(Device * device, float dt);
	TargetingSystemDef * definition;
	TrackingInfo info;
};

class TargetingSystemDef : public DeviceDef
{
protected:
	TurretDriver driver;
	friend class TargetingSystem;
public:
	TargetingSystemDef(DeviceManager &m):DeviceDef(m)
	{
		invokerTarget(0,HGEK_LBUTTON,-1,-1);
	}
	Device * create(IO::StreamIn *context=NULL)
	{
		return new TargetingSystem(this);
	}
};
/*
class TurretDriverDef: public DeviceDef
{
public:
	bool trackMoving;
	float error;		// mimimal aiming error, [deg]

	TurretDriverDef(DeviceManager &m):DeviceDef(m),error(1.0f)
	{
		invokerTarget(TurretDriver::portTarget,HGEK_LBUTTON,0,0);
	}
	Device * create(IO::StreamIn *context=NULL)
	{
		return new TurretDriver(this,context);
	}
};
*/