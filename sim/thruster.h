#ifndef THRUSTER_INCLUDED
#define THRUSTER_INCLUDED
#include "../sim/mover.h"
//////////////////////////////////////////////////////////////////////////
// Rocket thruster
//////////////////////////////////////////////////////////////////////////
class ThrusterDef;

class Thruster : public Device
{
public:
	enum {portForce = 0};
	float force;                      /// current thrusting force
	ThrusterDef * definition;
	Thruster(ThrusterDef * def);

	bool validCommand(int port,DeviceCmd cmd)const;
	int execute_Toggle(int port,DeviceCmd subtype);

	int readState(IO::StreamIn &buffer);
	int writeState(IO::StreamOut &buffer);
	DeviceDef * getDefinition();
	void update(float dt);
};

class ThrusterDef : public DeviceDef
{
public:
	float force;
	ThrusterDef(DeviceManager &manager):DeviceDef(manager)
	{
		force = 1.0f;
		invokerToggle(0,HGEK_W);
	}
	Device * create(IO::StreamIn * buffer)
	{
		return new Thruster(this);
	}
};

//////////////////////////////////////////////////////////////////////////
// Thruster control system
//////////////////////////////////////////////////////////////////////////
class ThrusterControlDef;

class ThrusterControl: public Mover
{
public:
	ThrusterControl(ThrusterControlDef * def);
	void directionControl(const Pose::vec & direction, float speed);
	//virtual float getMaxVelocity(int dir) const { return dir? definition->speedLinear : definition->speedAngular; }
	bool validCommand(int port,DeviceCmd cmd)const;
	int readState(IO::StreamIn &buffer);
	int writeState(IO::StreamOut &buffer);
	DeviceDef * getDefinition();
	void update(float dt);	
protected:
	ThrusterControlDef * definition;
};

class ThrusterControlDef : public MoverDef
{
public:
	ThrusterControlDef(DeviceManager &manager):MoverDef(manager){}
	Device * create(IO::StreamIn * buffer)
	{
		return new ThrusterControl(this);
	}
};

#endif