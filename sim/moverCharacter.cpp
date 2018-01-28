#include "moverCharacter.h"
#include "device.h"
#include "mover.h"

namespace sim
{
#ifdef FUCK_THIS
///////////////////////////////////////////////////////////////////////
// Definition
///////////////////////////////////////////////////////////////////////
MoverCharacterDef::MoverCharacterDef(DeviceManager &parent)
	:MoverDef(parent), ready(false), kinematic(true)
{
	autorotate = false;
	speedLinear=700;
	speedAngular=400;
	thrust = 1000;
	torque = 100;
}

void MoverCharacterDef::build()
{
	if(pathProcess)
	{
		pathProcess->setAdjacency(8);
		pathProcess->setActionCost(0,1);
		pathProcess->setActionLimit(0,255);
		pathProcess->setTurnRadius(1);	
		pathProcess->setSize(2.0f);			// size in local cells
		pathProcess->enable(pathProject::ppUseHeuristics,false);
		pathProcess->enable(pathProject::ppUseGenericBounds,true);
	}
	ready = true;
}


Device * MoverCharacterDef::create(IO::StreamIn *context)
{	
	if(!ready)build();	
	return new MoverCharacter(this);
}

#endif

MoverCharacter::MoverCharacter(MoverCharacter* def)
:Mover(def)
{
	prototype = def;
	velocityAngular = 0;
	velocity = vec2f(0,0);

	autorotate = false;
	speedLinear=700;
	speedAngular=400;
	thrust = 1000;
	torque = 100;

	setDriver(createVODriver(this));
}
MoverCharacter::~MoverCharacter()
{}

// Why not?
//
//int MoverCharacter::writeDesc(IOBuffer &context)
//{
//	Mover::writeDesc(context);
//	context.write(speedLinear);
//	context.write(speedAngular);
//	//if(b2Body)context->write(b2Body->getID());
//	return 0;//sizeof(ID);
//}
//int MoverCharacter::readDesc(IOBuffer &context)
//{
//	Mover::writeDesc(context);
//	context.read(speedLinear);
//	context.read(speedAngular);
//	return 0;
//}

/*
DeviceDef * MoverCharacter::getDefinition()
{
	return definition;
}*/

int MoverCharacter::writeState(StreamOut &buffer)
{
	Mover::writeState(buffer);
	buffer.write(velocity);
	buffer.write(velocityAngular);
	return 1;
}

int MoverCharacter::readState(StreamIn &buffer)
{
	Mover::readState(buffer);
	buffer.read(velocity);
	buffer.read(velocityAngular);
	return 1;
}

void MoverCharacter::setVelocity(float x,float y,float z,float vel)
{
	setVelocity(Pose::pos(x,y),vel);
}

void MoverCharacter::setVelocity(const Pose::pos &dir,float vel)
{
	toSync(true);

	Pose::pos velnew = Pose::pos(0.f);

	if(vecLength(dir)>0.0f)
		velnew = vec2f::normalize_s(dir) * math3::clamp<float>(vel, 0, speedLinear);

	if(velocity == velnew)
	{
		toSync(true);
		state=Moving;
	}
	velocity=velnew;
}

void MoverCharacter::directionControl(const vec2f &dir, float speed)
{
	setVelocity(dir, speedLinear);
	// 4. calculate prefered direction
	/*
	float forward=(dir & getDirection());
	float angle=dir.length_squared()>0.f?vecAngle2d_CW(dir,getDirection()):0.f;//,vecAngle2d_CW(sumForce,unit->getDirection())};
	if(angle>M_PI)
		angle-=(M_PI+M_PI);
	forward=clamp(deadZone(forward,-0.1f,0.1f),-1.f,1.f);
	angle=fSign(deadZone(angle,-0.1f,0.1f));
	
	//angle=5*deadZone(angle,-0.1f,0.1f);
	if(fabs(forward)>1.f)forward=0.f;
	if(fabs(angle)>1.f)angle=0.f;
	query_Direction(portLinear,dcmdDir_set, forward);	
	query_Direction(portAngular,dcmdDir_set, angle);
	// 5. apply control
	*/
}

bool MoverCharacter::validCommand(int port,DeviceCmd cmd)const
{
	return false;
}

float MoverCharacter::getMaxVelocity(int dir) const
{
	return dir? speedLinear : speedAngular;
}

void MoverCharacter::update(float dt)
{
	if(driver)
		driver->update(dt);
	/*
	if(state==Idle)
	{
		if(!definition->kinematic)
		{}
		else
		{
			getBody()->SetLinearVelocity(b2Vec2(0.f,0.f));
			getBody()->SetAngularVelocity(0);
		}
	}
	else if(state==Moving)*/
	{	
		if(!kinematic)
		{
			getBody()->ApplyForce(b2conv(velocity),getBody()->GetPosition());
			getBody()->ApplyTorque(velocityAngular);
		}
		else
		{
			getBody()->SetLinearVelocity(b2conv(velocity));
			getBody()->SetAngularVelocity(velocityAngular);
		}
	}
}

} // namespace sim
