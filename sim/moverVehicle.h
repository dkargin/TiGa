#pragma once

#include "mover.h"

namespace sim
{

/**
 * Kinematics class for car-like objects
 */
class MoverVehicle: public Mover
{
public:
#ifdef FUCK_THIS
	class Definition: public Mover::Definition
	{
		bool ready;
	public:
		Definition(DeviceManager &parent);
		~Definition();	
		vec2f maxVelocity;	// max linear\angular velocity
		bool kinematic;			// use force or velocity control
		bool moveBack;			// can move back - for 2d models of plane or other simular flying
		bool staticTurn;		// ����� �� �������������� �� �����
		vec2f acceleration;	// max linear\angular acseleration
		vec2f brakeForce;	// brakes force\torque
		void build();
		int init();

		inline const char * luaName()const
		{
			return "MoverVehicle";
		}

		Device * create(StreamIn * buffer);
	};
	typedef Definition MoverVehicleDef;
#endif

	enum 
	{
		portLinear=0,
		portAngular=1
	};	
public:
	MoverVehicle(MoverVehicle* def,StreamIn* data);
	bool validCommand(int port,DeviceCmd cmd)const;
	// sync control
	
	int readDesc(IOBuffer& context);
	int writeDesc(IOBuffer& context);
	int readState(StreamIn& buffer);
	int writeState(StreamOut& buffer);

	int execute_Direction(int port,DeviceCmd cmd,float val);
	virtual void directionControl(const Pose::vec& direction, float speed);
	virtual void update(float dt);

	virtual float getMaxVelocity(int dir) const;

	float currentVelocity[2];
protected:	
	
	float turning;		// {-1,0,1}
	float accelerate;	// {-1,0,1}

	// Moved from definition
	vec2f maxVelocity;	// max linear\angular velocity
	bool kinematic;			// use force or velocity control
	bool moveBack;			// can move back - for 2d models of plane or other simular flying
	bool staticTurn;		// ����� �� �������������� �� �����
	vec2f acceleration;	// max linear\angular acseleration
	vec2f brakeForce;	// brakes force\torque
	
	friend class Driver;
private:
	MoverVehicle* prototype;
};

/**
 * Implements flocking-based algorithm for movement control
 */
class MoverFlockingAI: public Mover::Driver
{
public:
	MoverFlockingAI(Mover * mover);

	virtual void update(float dt) override;
	virtual void render(Fx::RenderContext* rc) override;
	/// just for debug rendering
	std::list<std::pair<Pose::pos,Pose::pos> > pairs;
protected:

	// Calculates a virtual force to evade obstacles
	virtual vec2f forceObstacles();
	// Calculates a virtual force to follow the path
	virtual vec2f forcePath();
	// Calculates a virtual force to follow the leader
	virtual vec2f forceLeader();

	vec2f fObstacles,fPath,fLeader;
};

MoverVehicle * toVehicle(Mover * mover);

} // namespace sim
