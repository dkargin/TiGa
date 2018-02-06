#include "thruster.h"


namespace sim
{

Thruster::Thruster(Thruster* def)
  :force(0), Device(def)
{
	forceMax = 0.0;
	if(def)
	{
		forceMax = def->forceMax;
	}
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
		force = forceMax;
	if( cmd == dcmdToggle_off)
		force = 0.f;
	return 1;
}

int Thruster::writeState(StreamOut &buffer)
{
  buffer.write(force);
	return 1;
}

int Thruster::readState(StreamIn &buffer)
{
	buffer.read(force);
	return 1;
}

void Thruster::update(float dt)
{
	if(force > forceMax)
		force = forceMax;

  Pose pose = getGlobalPose();
  b2Body * body = getBody();
  body->ApplyForce(b2conv(force*pose.getDirection()),b2conv(pose.position));  
}


/*

���� �������� ������������ ����

*/

ThrusterControl::ThrusterControl(ThrusterControl* proto)
	:Mover(proto)
{
}

int ThrusterControl::readState(StreamIn & buffer)
{
	return 0;
}
int ThrusterControl::writeState(StreamOut & buffer)
{
	return 0;
}
void ThrusterControl::update(float dt)
{}


bool ThrusterControl::validCommand(int port, DeviceCmd cmd)const
{
  return false;//port == portForce && (cmd==dcmdDir || cmd==dcmdDir_inc || cmd==dcmdDir_set);
}

void ThrusterControl::directionControl(const vec2f & dir, float speed)
{
}

}
