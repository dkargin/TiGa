#pragma once

#include "mover.h"

//////////////////////////////////////////////////////////////////////////
// Rocket thruster
//////////////////////////////////////////////////////////////////////////
namespace sim
{

class Thruster : public Device
{
public:
	enum {portForce = 0};
	float force;                      /// current thrusting force
	float forceMax;

	Thruster(Thruster* prototype);

	bool validCommand(int port, DeviceCmd cmd) const;
	int execute_Toggle(int port, DeviceCmd subtype);

	int readState(StreamIn &buffer);
	int writeState(StreamOut &buffer);
	//DeviceDef * getDefinition();
	void update(float dt);
};

#ifdef FUCK_THIS

class ThrusterDef : public DeviceDef
{
public:
	float force;
	ThrusterDef(DeviceManager &manager):DeviceDef(manager)
	{
		force = 1.0f;
		invokerToggle(0,HGEK_W);
	}
	Device * create(StreamIn * buffer)
	{
		return new Thruster(this);
	}
};

#endif

/**
 * Thruster control system
 * Generally there should be only one such system on a unit
 */
class ThrusterControl: public Mover
{
public:
	ThrusterControl(ThrusterControl* proto);
	void directionControl(const Pose::vec& direction, float speed);
	bool validCommand(int port, DeviceCmd cmd)const;
	int readState(StreamIn& buffer);
	int writeState(StreamOut& buffer);
	void update(float dt);	
};

}
