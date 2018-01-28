#include "moverVehicle.h"
#include "mover.h"
#include "unit.h"

namespace sim {

#ifdef FUCK_THIS
////////////////////////////////////////////////////////////
//// Definition
////////////////////////////////////////////////////////////
MoverVehicle::Definition::Definition(DeviceManager &parent)
:MoverDef(parent),ready(false)
{
	maxVelocity[0] = 100;
	maxVelocity[1] = 4;
	moveBack=true;

	kinematic = false;

	acceleration[0] = 100;
	acceleration[1] = 10;

	invokerParam(0,HGEK_W,HGEK_S);
	invokerParam(1,HGEK_D,HGEK_A);
}

MoverVehicle::Definition::~Definition()
{}

void MoverVehicle::Definition::build()
{
	if(pathProcess && !ready)
	{
		adjacency = 1;
		pathProcess->setAdjacency(adjacency);
		pathProcess->setActionCost(0,1);
		pathProcess->setActionLimit(0,255);
		pathProcess->setTurnRadius(1);	
		pathProcess->setSize(20.0f);			// size in local cells
		smooth = false;
		pathProcess->enable(pathProject::ppUseSmoothTurns,smooth);
		heuristic = false;
		pathProcess->enable(pathProject::ppUseHeuristics,heuristic);
		bounds = false;
		pathProcess->enable(pathProject::ppUseGenericBounds,bounds);
		pathProcess->getCore()->setHeuristicMode(pathProject::heuristicNone);
	}
	ready=true;
}

Mover::Driver * createFlockingDriver(MoverVehicle *m);
Mover::Driver * createRVO2Driver(Mover* m);

Device * MoverVehicle::Definition::create(StreamIn * buffer)
{
	build();
	return new MoverVehicle(this,buffer);
}

#endif
//////////////////////////////////////////////////////////
// Object
//////////////////////////////////////////////////////////
MoverVehicle::MoverVehicle(MoverVehicle* def, StreamIn *data)
:Mover(def),prototype(def)
{
	turning = 0;
	accelerate = 0;
	currentVelocity[0]=0;
	currentVelocity[1]=0;
	moveBack = true;
	kinematic = true;
	staticTurn = true;

	//auto driver = createVehicleDriver(this);
	//auto driver = createFlockingDriver(this);
	//auto driver = createVO2Driver(this);
	//auto driver = createVODriver(this);
	//auto driver = createRVO2Driver(this);

	//setDriver(driver);
}

int MoverVehicle::writeState(StreamOut &buffer)
{
	Mover::writeState(buffer);
	buffer.write(accelerate);
	buffer.write(turning);
	return 1;
}

int MoverVehicle::readState(StreamIn &buffer)
{
	Mover::readState(buffer);
	buffer.read(accelerate);
	buffer.read(turning);
	return 1;
}

bool MoverVehicle::validCommand(int port, DeviceCmd cmd)const
{
	return (port==portLinear || port==portAngular) && cmdIsDirection(cmd);
}

int MoverVehicle::execute_Direction(int port, DeviceCmd action,float value)
{
	if(port==portLinear)
	{
		if(action==dcmdDir_inc)
			accelerate+=value;
		else if(action==dcmdDir_set)
			accelerate=value;
		else
			return dcmdWrongType;
	}
	else if(port==portAngular)
	{
		if(action==dcmdDir_inc)
			turning+=value;
		else if(action==dcmdDir_set)
			turning=value;
		else
			return xWrongCmd();
	}
	else
		return xWrongPort();

	accelerate = math3::clamp<float>(accelerate,-1.0f,1.0f);
	turning = math3::clamp<float>(turning,-1.0f,1.0f);
	assert(fabs(accelerate)<=2.f);
	assert(fabs(turning)<=2.f);
	return dcmdOk;
}

void MoverVehicle::directionControl(const vec2f &dir, float speed)
{
	float t = 0.5;
	if(speed == 0.f)
		t = 1.0;
	Pose::vec unitDirection = getMaster()->getDirection();
	float forward = 0, angle = 0;
	// 1. Interpolate control
	Pose::vec control = dir * t + (1.f - t) * lastControl;
	lastControl = control;
	// 2. Calculate drive control for kinematics mode
	if(kinematic)
	{
		//float errorAngle = dir.length_squared() > 0.f ? vecAngle2d_CW(dir,unitDirection) : 0.f;//,vecAngle2d_CW(sumForce,unit->getDirection())};
		float error[] = 
		{
			(speed - currentVelocity[0]) * math3::fSign(unitDirection & control),
			dir.length_squared() > 0.f ? vecAngle2d_CW(control,unitDirection) : 0.f,
		};
	//	// 4. calculate prefered direction
		if(error[1] > M_PI)
			error[1] -= (2 * M_PI);
		float turnTime = fabs(error[1] / currentVelocity[1]);
		float brakingTime = sqrt(2 * fabs(error[1]) / acceleration[1]);
		if( currentVelocity[1] * error[1] >= 0.0)
		{
			
			if(turnTime < brakingTime)
				angle = -error[1];
			else
				angle = math3::fSign(error[1]);

			//if(fabs(turnTime) < 0.1)
			//	angle = 0;
		}
		else
		{
			angle = (error[1] >= 0) ? 1.f : -1.f;
		}
		//angle = fSign(errorAngle);
		//angle = errorAngle;
		//angle = -fSign(currentVelocity[1]);
		float errorForward = math3::deadZone(error[0], -0.1f, 0.1f);
		forward = math3::clamp<float>(errorForward,-1.f,1.f);
		//angle = fSign(deadZone(angle,-0.1f,0.1f));
		angle = math3::clamp<float>(angle,-1.f,1.f);
	}
	else
	{
		// 4. calculate prefered direction
		forward = speed;//(dir & unitDirection);
		angle = control.length_squared()>0.f?vecAngle2d_CW(control,unitDirection):0.f;//,vecAngle2d_CW(sumForce,unit->getDirection())};

		if(angle > M_PI)
			angle -= (M_PI+M_PI);

		float errorForward = math3::deadZone(speed, -0.1f, 0.1f);
		forward = math3::clamp<float>(errorForward,-1.f,1.f);
		float errorAngle = math3::deadZone(angle,-0.1f,0.1f);
		angle = math3::fSign(errorAngle);
	}

	// 4. fix values. Why do we reset it to zero?
	if(fabs(forward)>1.f)
		forward = 0.f;
	if(fabs(angle)>1.f)
		angle = 0.f;	
	// 5. apply control
	query_Direction(portLinear,dcmdDir_set, forward);	
	query_Direction(portAngular,dcmdDir_set, angle);
}

void MoverVehicle::update(float dt)
{
	auto body=getBody();
	Pose pose = getGlobalPose();
	
	vec2f vel = conv(getBody()->GetLinearVelocity());	

	vec2f X = pose.axisX();
	vec2f Y = pose.axisY();
	
	float totalVel = vel.length();
	currentVelocity[0] = vel & X;
	currentVelocity[1] = getBody()->GetAngularVelocity();
	const float strafeScale = 0.9;
	float strafe = strafeScale * (vel & Y);

	if(driver && deviceMode == dmDriver)
		driver->update(dt);

#ifdef FUCK_THIS
	if(definition->pathProcess)
	{
		float size = this->getMaster()->getBoundingSphere().radius;
		definition->pathProcess->setSize(size);
	}
#endif
/*
	if(state == Idle)
	{
		accelerate = 0.f;
		turning = 0.f;
	}
*/
	
	if(kinematic)
	{		
		b2Body * body = getBody();
		b2MassData massData;
		body->GetMassData(&massData);

		float linearDamping = acceleration[0]/maxVelocity[0]; //*definition->maxVelocity[0]);
		float angularDamping = acceleration[1]/maxVelocity[1]; //*definition->maxVelocity[1]);
		body->SetLinearDamping(linearDamping);
		body->SetAngularDamping(angularDamping);

		if(true)
		{
			// Using force model
			body->ApplyForce(b2conv(accelerate*acceleration[0] * massData.mass *pose.getDirection()),b2conv(pose.position));
			body->ApplyTorque(turning*acceleration[1] * massData.I);
			// Using impulse model
			//body->ApplyLinearImpulse(b2conv((accelerate/dt)*definition->acceleration[0] * massData.mass *pose.getDirection()),b2conv(pose.position));
			//body->ApplyAngularImpulse((turning/dt)*definition->acceleration[1] * massData.I);
		}
		else
		{
			body->ApplyForce(b2conv(accelerate*acceleration[0] *pose.getDirection()),b2conv(pose.position));
			body->ApplyTorque(turning*acceleration[1]);
		}
		// Comlensate steering - apply friction analog
		body->ApplyLinearImpulse(b2conv(-strafe * Y * massData.mass), b2conv(pose.position));
		if(getState() == Idle)
		{
			//body->ApplyLinearImpulse(b2conv( -currentVelocity[0] * X * massData.mass), b2conv(pose.position));
			//body->ApplyAngularImpulse(-currentVelocity[1] * massData.I);
		}
	}
	else
	{
		vec2f limit = accelerate*maxVelocity[0]*pose.getDirection();
		body->SetLinearVelocity(b2conv(limit));
		body->SetAngularVelocity(turning*maxVelocity[1]);
	}
}

float MoverVehicle::getMaxVelocity(int dir) const
{
	return maxVelocity[dir];
}

//////////////////////////////////////////////////////////
// Manager/Factory
//////////////////////////////////////////////////////////
MoverVehicle * toVehicle(Mover * mover)
{
	return dynamic_cast<MoverVehicle*>(mover);
}

}
