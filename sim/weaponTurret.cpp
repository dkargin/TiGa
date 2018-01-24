#include "../sim/weaponTurret.h"

#include "../sim/projectile.h"
#include "../sim/unit.h"
#include "stdafx.h"

///////////////////////////////////////////////////////
// Turret weapon
///////////////////////////////////////////////////////

WeaponTurret::WeaponTurret(WeaponTurretDef *def)
:Weapon(def),direction(0),angle(0),definition(def)
{}

WeaponTurret::~WeaponTurret()
{}
/*
int WeaponTurret::readDesc(IOBuffer &buffer)
{
	Weapon::readDesc(buffer);
	buffer.read(velocity);
	buffer.read(dimensions);
	return 1;
}
int WeaponTurret::writeDesc(IOBuffer &buffer)
{
	Weapon::writeDesc(buffer);
	buffer.write(velocity);
	buffer.write(dimensions);
	return 1;
}*/

float WeaponTurret::getDimensions() const 
{ 
	return definition->dimensions; 
}

bool WeaponTurret::validCommand(int port,DeviceCmd cmd)const
{
	return (port==portTurn && cmdIsDirection(cmd)) ? true:Weapon::validCommand(port,cmd);
}
int WeaponTurret::execute_Direction(int port,DeviceCmd action,float value)
{
	if(port==portTurn && action==dcmdDir_inc)
		direction+=value;
	else if(port==portTurn && action==dcmdDir_set)
		direction=value;
	else
		return xWrongPort();
	direction=clampf(direction,-1.0,1.0);
	return dcmdOk;
}

Pose WeaponTurret::getMuzzlePose()
{
	return getGlobalPose() * Pose(vec2f(0.0f),getCurrentAngle()*3.1415/180.0) * Pose(vec2f(weaponData.muzzleOffset,0),0);
}

void WeaponTurret::update(float dt)
{
	Weapon::update(dt);
	effects.setPose(pose * Pose(vec2f(0,0),getCurrentAngle()*3.1415/180.0));
	
	angle+=(direction * definition->velocity*dt);
	if(angle<-180)
		angle+=360;
	if(angle> 180)
		angle-=360;
	if(angle>definition->dimensions)
		angle=definition->dimensions;
	if(angle<-definition->dimensions)
		angle=-definition->dimensions;	
}

float WeaponTurret::getCurrentAngle() const
{
	return angle;
}

float WeaponTurret::getAngleTo(const Pose::pos &vec) const
{
	Pose::pos v = getGlobalPose().projectPos(vec);
	return atan2(v[1],v[0])*180.0f/M_PI;
}

bool WeaponTurret::canReach(const Pose::pos &v) const
{
	float angle = getAngleTo(v);
	return fabs(angle) < definition->dimensions;
}

bool WeaponTurret::fullTurn() const
{
	return definition->dimensions>=180;
}
int WeaponTurret::writeState(IO::StreamOut &buffer)
{
	Weapon::writeState(buffer);
	//buffer.write(time);
	buffer.write(angle);
	buffer.write(direction);
	return 1;
}

int WeaponTurret::readState(IO::StreamIn &buffer)
{
	Weapon::readState(buffer);
	//buffer.read(time);
	buffer.read(angle);
	buffer.read(direction);
	return 1;
}
void WeaponTurret::stop()
{
	query_Direction(portTurn,dcmdDir_set,0);
}
DeviceDef * WeaponTurret::getDefinition()
{
	return definition;
}
/////////////////////////////////////////////////////////////////////
//// Weapon autodriver object
/////////////////////////////////////////////////////////////////////
TurretDriver::TurretDriver()
:minError(0.4)
{}
TurretDriver::~TurretDriver()
{}

vec2f getWeaponTarget2(const TrackingInfo &info, const vec2f &pos,ProjectileDef *def);

bool TurretDriver::aim(WeaponTurret * weapon, const TrackingInfo& info, float dt)
{	
	const Pose & pose = weapon->getGlobalPose();
	vec2f aimPos = getWeaponTarget2(info, pose.getPosition(), weapon->definition->projectile); 
	float targ = weapon->getAngleTo(aimPos);
	float curr = weapon->getCurrentAngle();
	float deltaAngle = targ-curr;
	float deltaVel = dt*weapon->definition->velocity;
	if(weapon->fullTurn())
	{
		if(deltaAngle>= 180)deltaAngle-=360;
		if(deltaAngle<=-180)deltaAngle+=360;
	}	
	
	float angle = fSign(deltaAngle);
	if(deltaAngle < deltaVel)
		angle = deltaAngle/deltaVel;

	weapon->query_Direction(weapon->portTurn,dcmdDir_set,angle);	
	
	if(fabs(deltaAngle) < minError)
	{		
		float distance = vecDistance(info.pos,weapon->getMuzzlePose().getPosition());
		if(distance <= weapon->getEffectiveRange())
			return true;		
	}
	return false;
}

bool flagTest(unsigned int value,unsigned int f)
{
	return value & f;
}

void flagSet(unsigned int &value,unsigned int f,bool v)
{
	value=v ? (value|f) : (value&~f);
}
///////////////////////////////////////////////////////////////////////
WeaponTurret *toTurret(Device * weapon)
{	
	return dynamic_cast<WeaponTurret*>(weapon);
}
///////////////////////////////////////////////////////////////////////


TargetingSystem::TargetingSystem(TargetingSystemDef * def)
	:definition(def),Device(def)
{
	deviceMode = dmOffline;
}

TargetingSystem::~TargetingSystem()
{}

DeviceDef * TargetingSystem::getDefinition()
{
	return definition;
}

int TargetingSystem::writeState(IO::StreamOut &buffer)
{
	return 1;
}

int TargetingSystem::readState(IO::StreamIn &buffer)
{
	return 1;
}

void TargetingSystem::checkDevices()
{
	takeControl(NULL);
}

bool TargetingSystem::canControl(Device * device) const
{
	return dynamic_cast<WeaponTurret*>(device)!=NULL;
}

bool TargetingSystem::validCommand(int port,DeviceCmd cmd)const
{
	return port == portTarget && cmdIsTarget(cmd);
}

int TargetingSystem::execute_Target(int port,DeviceCmd subtype,const Pose::pos &target)
{
	if(port != portTarget)
		return 0;
	if(subtype == dcmdTarget_set)
	{
		info.pos = target;
		deviceMode = dmDriver;
		/// TODO: extract object size
		info.size = 5;	
	}
	else if(subtype == dcmdTarget_reset)
	{
		deviceMode = dmOffline;
	}
	return 1;
}

void TargetingSystem::onInstall(Unit * master, size_t id, const Pose &pose)
{
	Device::onInstall(master,id,pose);
	std::for_each(master->devices.begin(),master->devices.end(),[&](Device * device)
	{
		if(device != this && canControl(device))
			takeControl(device);
	});
}

void TargetingSystem::onInstallDevice( Device::Pointer device, int mount)
{
	takeControl(device);
}

bool TargetingSystem::executeControl( Device * device, float dt)
{
	auto * turret = dynamic_cast<WeaponTurret*>(device);
	if(!turret)
		return false;
	if(deviceMode == dmOffline)
		turret->stop();
	else if(definition->driver.aim(turret,info,dt))
		turret->shoot();
	return true;
}