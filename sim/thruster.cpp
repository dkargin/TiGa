#include "../sim/thruster.h"

#include "stdafx.h"


Thruster::Thruster(ThrusterDef * def)
  :definition(def),force(0),Device(def)
{}
DeviceDef * Thruster::getDefinition()
{
	return definition;
}
bool Thruster::validCommand(int port,DeviceCmd cmd)const
{
  return port == portForce && cmdIsToggle(cmd);//(cmd==dcmdDir || cmd==dcmdDir_inc || cmd==dcmdDir_set);
}
int Thruster::execute_Toggle(int port,DeviceCmd cmd)
{
	if(port != portForce)
		return 0;
	if( cmd == dcmdToggle_on)
		force = definition->force;
	if( cmd == dcmdToggle_off)
		force = 0.f;
	return 1;
}
int Thruster::writeState(IO::StreamOut &buffer)
{
  buffer.write(force);
	return 1;
}

int Thruster::readState(IO::StreamIn &buffer)
{
	buffer.read(force);
	return 1;
}

void Thruster::update(float dt)
{
  force *= Clamp(0,definition->force);
  Pose pose = this->getGlobalPose();
  b2Body * body = getBody();
  body->ApplyForce(b2conv(force*pose.getDirection()),b2conv(pose.position));  
}


/*

���� �������� ������������ ����

*/

ThrusterControl::ThrusterControl(ThrusterControlDef * def)
	:definition(def),Mover(def)
{
}

DeviceDef * ThrusterControl::getDefinition()
{
	return definition;
}

int ThrusterControl::readState(IO::StreamIn & buffer)
{
	return 0;
}
int ThrusterControl::writeState(IO::StreamOut & buffer)
{
	return 0;
}
void ThrusterControl::update(float dt)
{}


bool ThrusterControl::validCommand(int port,DeviceCmd cmd)const
{
  return false;//port == portForce && (cmd==dcmdDir || cmd==dcmdDir_inc || cmd==dcmdDir_set);
}

void ThrusterControl::directionControl(const vec2f & dir, float speed)
{
}