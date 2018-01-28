#pragma once

#include "mover.h"
/////////////////////////////////////////////////////////////
// ��������� ������ ����������, ������ �� ���������� � �����������
// ����������� ������� �������� � ���������� ������� ���������,
// ����������� �������� �������� ��������� � ������������ ��������
// ��� ���������� �������� �� ����� �������� ����
// ��� ���������� �������� �� ����� �������� 2� ������
/////////////////////////////////////////////////////////////

namespace sim
{

class MoverVehicle: public Mover
{	
public:	
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
		Device * create(IO::StreamIn * buffer);
	};
	typedef Definition MoverVehicleDef;
	Definition *definition;
	enum 
	{
		portLinear=0,
		portAngular=1
	};	
public:
	MoverVehicle(Definition *def,IO::StreamIn * data);
	//MoverVehicle(DeviceManager *manager);
	bool validCommand(int port,DeviceCmd cmd)const;
	// sync control
	
	int readDesc(IOBuffer &context);
	int writeDesc(IOBuffer &context);	
	int readState(IO::StreamIn &buffer);
	int writeState(IO::StreamOut &buffer);

	int execute_Direction(int port,DeviceCmd cmd,float val);
	virtual void directionControl(const Pose::vec& direction, float speed);
	virtual void update(float dt);

	DeviceDef * getDefinition();
	MoverVehicleDef * localDefinition()const
	{
		return definition;
	}
	virtual float getMaxVelocity(int dir) const { return definition->maxVelocity[dir]; }
	//float velocity[2];

	float currentVelocity[2];
protected:	
	
	float turning;		// {-1,0,1}
	float accelerate;	// {-1,0,1}		
	
	friend class Driver;
};
typedef MoverVehicle::Definition MoverVehicleDef;
/////////////////////////////////////////////////////////////////////////
class DriverFlocking: public Mover::Driver
{
public:
	DriverFlocking(Mover * mover);
	virtual void update(float dt);
#ifdef DEVICE_RENDER
	virtual void render(HGE * hge);
#endif
	std::list<std::pair<Pose::pos,Pose::pos> > pairs;	/// just for debug rendering
protected:
	virtual vec2f forceObstacles();	// calculate force to evade obstacles
	virtual vec2f forcePath();		// calculate force to follow the path
	virtual vec2f forceLeader();		// calculate force to follow the leader
	vec2f fObstacles,fPath,fLeader;
};

MoverVehicle * toVehicle(Mover * mover);

}
