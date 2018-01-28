#pragma once

#include "unit.h"
#include "weapon.h"

namespace sim
{

class Unit;
class WeaponTurret;
//class TurretDriver;

class WeaponTurret: public Weapon//, virtual public ProtoImpl<WeaponTurret,Device>
{
	friend class TurretDriver;	
	float angle;		// angle in degrees
	float direction;	// -1,0,1
	
	// From definition
	float velocity;
	float dimensions;

	//WeaponTurretDef * definition;
public:	
	enum {portTurn=1};

	WeaponTurret(WeaponTurret* proto=nullptr);
	~WeaponTurret();

	float getDimensions() const ;
	void stop();
	//int writeDesc(IOBuffer &context);
	//int readDesc(IOBuffer &context);
	int writeState(StreamOut &buffer);
	int readState(StreamIn &buffer);

	virtual void update(float dt);	
	virtual float getCurrentAngle() const;					// get current muzzle angle
	virtual float getAngleTo(const Pose::pos &v) const;	// get angle to a given vector
	virtual Pose getMuzzlePose();
	bool canReach(const Pose::pos &v) const;				// if vector is in reachable zone
	bool fullTurn()const;							// if region [-180,180] is reachable
	
	//DeviceDef * getDefinition();

	bool validCommand(int port, DeviceCmd cmd) const;
	int execute_Direction(int port, DeviceCmd action, float value);
};

#ifdef FUCK_THIS
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
	Device * create(StreamIn *context=NULL)
	{
		return new WeaponTurret(this);
	}
};

#endif

// lua helper functions
WeaponTurret* toTurret(Device * weapon);

struct TrackingInfo
{
	GameObjectPtr object;
	Pose::pos pos;	// target position
	Pose::vec vel;	// target velocity
	float size;		// target size
};

class TurretDriver
{

public:
	float minError = 0.4;

	virtual ~TurretDriver() {}
	virtual bool aim(WeaponTurret * weapon,const TrackingInfo& info,float dt);
};


/// gets cmdTarget
class TargetingSystem : public Device, public Unit::DeviceListener
{	
public:
	enum { portTarget = 0 };	

	TargetingSystem();
	~TargetingSystem();

	int writeState(StreamOut &buffer);
	int readState(StreamIn &buffer);

	void onInstall(Unit * unit,size_t id, const Pose & pose);

	//DeviceDef * getDefinition();
	bool validCommand(int port,DeviceCmd cmd)const;
	virtual bool canControl(Device * device) const;
	virtual int execute_Target(int port,DeviceCmd subtype,const Pose::pos &target);

	virtual void onInstallDevice(DevicePtr device, int id);

	virtual void checkDevices();
protected:
	virtual bool executeControl(Device * device, float dt);
	//TargetingSystemDef * definition;
	TrackingInfo info;
};

#ifdef FUCK_THIS
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
#endif
} // namespace sim
