#include "../sim/weaponTurret.h"

#include "../sim/projectile.h"
#include "../sim/unit.h"

namespace sim
{

///////////////////////////////////////////////////////
// Turret weapon
///////////////////////////////////////////////////////

WeaponTurret::WeaponTurret(WeaponTurret* def)
:Weapon(def),direction(0),angle(0)
{
	dimensions = 1.0;
	velocity = 1.0;
}

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
	return dimensions;
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

	direction = math3::clamp<float>(direction,-1.0,1.0);
	return dcmdOk;
}

Pose WeaponTurret::getMuzzlePose()
{
	return getGlobalPose() * Pose(vec2f(0.0f),getCurrentAngle()*3.1415/180.0) * Pose(vec2f(weaponData.muzzleOffset,0),0);
}

void WeaponTurret::update(float dt)
{
	Weapon::update(dt);

	fx_root->setPose(pose * Pose(vec2f(0,0),getCurrentAngle()*3.1415/180.0));
	
	angle += (direction * velocity*dt);
	if(angle<-180)
		angle+=360;
	if(angle> 180)
		angle-=360;

	if(angle > dimensions)
		angle = dimensions;

	if(angle < -dimensions)
		angle = -dimensions;
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
	return fabs(angle) < dimensions;
}

bool WeaponTurret::fullTurn() const
{
	return dimensions>=180;
}

int WeaponTurret::writeState(StreamOut &buffer)
{
	Weapon::writeState(buffer);
	//buffer.write(time);
	buffer.write(angle);
	buffer.write(direction);
	return 1;
}

int WeaponTurret::readState(StreamIn &buffer)
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

/////////////////////////////////////////////////////////////////////
//// Weapon autodriver object
/////////////////////////////////////////////////////////////////////

vec2f getWeaponTarget2(const TrackingInfo &info, const vec2f &pos, Projectile* def);

bool TurretDriver::aim(WeaponTurret* weapon, const TrackingInfo& info, float dt)
{	
	const Pose & pose = weapon->getGlobalPose();
	Projectile* proj = nullptr; //weapon->projectile
	vec2f aimPos = getWeaponTarget2(info, pose.getPosition(), proj);
	float targ = weapon->getAngleTo(aimPos);
	float curr = weapon->getCurrentAngle();
	float deltaAngle = targ-curr;
	float deltaVel = dt*weapon->velocity;
	if(weapon->fullTurn())
	{
		if(deltaAngle>= 180)deltaAngle-=360;
		if(deltaAngle<=-180)deltaAngle+=360;
	}	
	
	float angle = math3::fSign(deltaAngle);
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


TargetingSystem::TargetingSystem()
	:Device(nullptr)
{
	deviceMode = dmOffline;
}

TargetingSystem::~TargetingSystem()
{}

int TargetingSystem::writeState(StreamOut &buffer)
{
	return 1;
}

int TargetingSystem::readState(StreamIn &buffer)
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
	for(DevicePtr device: master->devices)
	{
		if(device != shared_from_this() && canControl(device.get()))
			takeControl(device);
	}
}

void TargetingSystem::onInstallDevice( DevicePtr device, int mount)
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
	else if(driver.aim(turret,info,dt))
		turret->shoot();
	return true;
}
} // namespace sim
