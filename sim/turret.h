#pragma once
#include "../sim/device.h"
#include "../sim/objectManager.h"
#include "assembly.h"

class Turret;
class TurretDef;
typedef DevicePair<TurretDef,Turret,BaseDevicePair> TurretPair;

class TurretDef: public TurretPair::Def, public AssemblyDef
{
public:
	float velocity;
	float dimensions;
	enum AILevel
	{
		None,
		Tracking,
		Prediction,
	}aiLevel;
	TurretDef(DeviceManager *m):Definition(m),velocity(0.0f),dimensions(0.0f){}
};

class TurretDriver;
class Weapon;

class Turret: public Device,public Assembly
{
	friend class TurretControl;
	float velocity;
	float dimensions;
	float angle;		// angle in degrees
	float direction;	// turning direction -1,0,1	
public:
	/*class iValue: public InterfaceValue
	{
	public:
		float value;
		Turret *parent;
		iValue(Turret *owner,float def):value(def),parent(owner){}

		float get()
		{
			return value;
		}
		void onIncrement(float inc)
		{
		}
		void onSet(float v)
		{
		}
		operator float()
		{
			return value;
		}
	}_direction;*/
	enum {portTurn=0};
	
	Turret(DeviceManager *manager,Device::BuildContext *context=NULL);
	~Turret();
	
	virtual void update(float dt);
	//virtual Interfaces getInterfaces();
	bool validCommand(int port,DeviceCmd cmd)const;
	virtual float getAngle() const;						// get current muzzle angle
	virtual float getAngle(const Pose::pos &v) const;	// get angle to a given vector
	bool canReach(const Pose::pos &v) const;			// if vector is in reachable zone
	bool fullTurn()const;								// if region [-180,180] is reachable
	int execute_Direction(int port,DeviceCmd action,float delta);		// executes and action
	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);
	
	/// from Assembly
	virtual void useDevice(int device,int port,int action,IOBuffer *buffer);
	virtual Pose::pos getPosition() const;
	virtual Pose::dir getDirection() const;
	virtual Pose getPose()const;

	virtual Unit * getUnit()
	{
		return master->getUnit();
	}
};

// lua helper functions
//WeaponTurret* toTurret(Weapon * weapon);
//WeaponTurret::Driver * getTurretDriver(Weapon * weapon);
//class TurretControl;
//class TurretControlDef;
//typedef DevicePair<TurretControlDef,TurretControl> TurretControlPair;
//
//class TurretControlDef: public TurretControlPair::Def
//{
//public:
//	bool trackMoving;
//	float error;		// mimimal aiming error, [deg]
//	TurretControlDef(DeviceManager *m):Definition(m),error(0.1),trackMoving(false){};
//};
//class TurretControl: public TurretControlPair::Obj
//{
//	Pose::pos target;
//	Turret *turret;
//	Weapon *weapon;
//	unsigned int lstate;
//	float minError;
//	bool usePrediction;
//public:
//	enum {portTarget=0};
//
//	enum
//	{
//		Wait=0,
//		Aim=1<<0,
//		AimOnce=1<<1,
//		Shoot=1<<2,
//		ShootOnce=1<<3
//	};
//	TurretControl(DeviceManager *turr,Device::BuildContext *context);
//	~TurretControl();
//	void stop();	// reset task
//
//	int writeState(IO::StreamOut &buffer){return 0;};
//	int readState(IO::StreamIn &buffer){return 0;};
//	bool canControl(Device *device);
//	bool validCommand(int port,DeviceCmd cmd)const;		
//	int execute_Target(int port,DeviceCmd action,const Pose::vec &vec);
//	void update(float dt);
//};