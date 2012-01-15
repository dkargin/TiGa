#include "stdafx.h"
//#include "world.h"
#include "turret.h"
#include "weapon.h"
#include "weaponTurret.h"
#include "device.h"
#include "assembly.h"
#include "unit.h"
#include "projectile.h"

bool flagTest(unsigned int value,unsigned int f);
void flagSet(unsigned int &value,unsigned int f,bool v);
///////////////////////////////////////////////////////
// Turret
///////////////////////////////////////////////////////
Turret::Turret(DeviceManager * manager,Device::BuildContext *context)
:Device(manager,context),Assembly(manager,context),direction(this,0),angle(0)
{}
Turret::~Turret()
{}
bool Turret::validCommand(int port,DeviceCmd cmd)const
{
	return port==portTurn && (cmd==dcmdDir || cmd==dcmdDir_inc || cmd==dcmdDir_set);
}
int Turret::execute_Direction(int port,DeviceCmd action,float value)
{
	if(port==portTurn && action==dcmdDir_inc)
		direction.onIncrement(value);
	else if(port==portTurn && action==dcmdDir_set)
		direction.onSet(value);
	else
		return xWrongPort();
	_direction.value=clampf(_direction,-1.0,1.0);
	return dcmdOk;
}

void Turret::update(float dt)
{
	effects.setPose(master->getMountPose(mount)*Pose(vec2f(0,0),getAngle()*3.1415/180.0));
	effects.update(dt);

	angle+=(_direction*velocity*dt);
	if(angle<-180)
		angle+=360;
	if(angle> 180)
		angle-=360;
	if(angle>dimensions)
		angle=dimensions;
	if(angle<-dimensions)
		angle=-dimensions;
}

float Turret::getAngle() const
{
	return angle;
}
float Turret::getAngle(const Pose::pos &vec) const
{
	Pose::pos v=getGlobalPose().projectPos(vec);
	return atan2(v[1],v[0])*180.0f/M_PI;
}
bool Turret::canReach(const Pose::pos &v) const
{
	float angle=getAngle(v);
	return fabs(angle)<dimensions;
}
bool Turret::fullTurn() const
{
	return dimensions>=180;
}
int Turret::writeState(IO::StreamOut &buffer)
{
	buffer.write(angle);
	buffer.write(_direction.value);
	return 8;
}
int Turret::readState(IO::StreamIn &buffer)
{
	buffer.read(angle);
	buffer.read(_direction.value);
	return 8;
}
void Turret::useDevice(int device,int port,int action,IOBuffer *buffer)
{
	//manager()->useDevice(this,device,port,action,buffer);
}
Device::Interfaces Turret::getInterfaces()
{
	Device::Interfaces result,tmp;
	for(auto it=mounts.begin();it!=mounts.end();++it)
	{
		Device *dev=it->device;
		if(!dev)continue;
		tmp=dev->getInterfaces();
		result.insert(result.end(),tmp.begin(),tmp.end());
	}
	return result;
}
Pose::pos Turret::getPosition() const
{
	return this->getGlobalPose().getPosition();
}
Pose::dir Turret::getDirection() const
{
	return this->getGlobalPose().getDirection();
}
Pose Turret::getPose()const
{
	return getGlobalPose();
}
///////////////////////////////////////////////////////////////////
// Weapon autodriver object
///////////////////////////////////////////////////////////////////
TurretControl::TurretControl(DeviceManager *manager,Device::BuildContext *context)
:Device(manager,context),lstate(AimOnce |ShootOnce)
{}
TurretControl::~TurretControl()
{}

bool TurretControl::canControl(Device *device)
{
	return dynamic_cast<Turret*>(device)!=NULL || dynamic_cast<Weapon*>(device)!=NULL;
}
//int TurretDriver::execute(DeviceCmd action,IOBuffer *buffer)
//{
//	if(weapon)return 0;
//	if(action==dcmdTargetPoint)
//	{
//		if(!buffer || buffer->eof())	
//			lstate=0;
//		else
//		{
//			buffer->read(target);
//			flagSet(lstate,Aim|Shoot,true);
//			//return fabs(weapon->getAngle(target)-weapon->getAngle())<error;
//		}
//	}
//	else
//		return 0;
//	return 1;
//}
bool TurretControl::validCommand(int port,DeviceCmd cmd)const
{
	return port==portTarget && cmd==dcmdTarget;//(cmd==dcmdTarget_set || cmd==dcmdTarget_update || cmd==dcmdTarget_reset);
}
int TurretControl::execute_Target(int port,DeviceCmd cmd,const Pose::vec &vec)
{
	if(port==portTarget)
	{
		if(cmd==dcmdTarget_set || cmd==dcmdTarget_update)
		{
			target=vec;
			flagSet(lstate,Aim|Shoot,true);
		}
		else if(cmd==dcmdTarget_reset)		// reset target
		{
			stop();
		}
		return dcmdOk;
	}
	return 0;
}
void TurretControl::stop()
{
	lstate=0;
	for(auto it=this->cmdOut.begin();it!=cmdOut.end();++it)
	{
		WeaponTurret *weapon=dynamic_cast<WeaponTurret*>(*it);
		weapon->query_Direction(weapon->portTurn,dcmdDir_set,0);
	}
}
void TurretControl::update(float dt)
{
	for(auto it=this->cmdOut.begin();it!=cmdOut.end();++it)
	{
		WeaponTurret *weapon=dynamic_cast<WeaponTurret*>(*it);
		//if(!weapon)return;
		float targ=weapon->getAngle(target);
		float curr=weapon->getAngle();
		float deltaAngle=targ-curr;
		float deltaVel=dt*turret->velocity;
		if(weapon->fullTurn())
		{
			if(deltaAngle>= 180)deltaAngle-=360;
			if(deltaAngle<=-180)deltaAngle+=360;
		}
		if(flagTest(lstate,Shoot) && fabs(deltaAngle) < minError)
		{		
			weapon->query_Action(weapon->portShoot);
			//if(flagTest(lstate,ShootOnce))flagSet(lstate,Shoot,false);		
		}
		if(flagTest(lstate,Aim))
		{
			//localBuffer.rewind();
			//localBuffer.write(fabs(deltaAngle)<deltaVel?deltaAngle/deltaVel:deltaAngle);
			//localBuffer.flip();
			weapon->query_Direction(weapon->portTurn,dcmdDir_inc,fabs(deltaAngle)<deltaVel?deltaAngle/deltaVel:deltaAngle);
			//weapon->query(DeviceCmd::dcmdParam,&localBuffer);
			//localBuffer.flip();
		}
	}
}

