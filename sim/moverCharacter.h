#pragma once

#include "mover.h"

namespace sim
{

#ifdef FUCK_THIS
struct MoverCharacterDef: public MoverDef
{
	float speedLinear;
	float speedAngular;
	float thrust,torque;	// force values for kinematic mode
	bool kinematic;
	bool autorotate;		// should we rotate automaticly
	//vec3 dimensions;		// bounds dimensions
	MoverCharacterDef(DeviceManager &parent);
	virtual Device * create(StreamIn *context);
private:
	void build();
	bool ready;
};

#endif

class MoverCharacter: public Mover
{
public:	
	MoverCharacter(MoverCharacter* def);
	~MoverCharacter();

	virtual void update(float dt) override;

	virtual void setVelocity(float x,float y,float z,float vel);
	virtual void setVelocity(const Pose::vec& dir, float vel);

	virtual void directionControl(const Pose::vec& direction, float speed);

	bool validCommand(int port,DeviceCmd cmd)const;

	virtual float getMaxVelocity(int dir) const;

	// sync control
	//int readDesc(IOBuffer &context);
	//int writeDesc(IOBuffer &context);	
	int readState(StreamIn& buffer) override;
	int writeState(StreamOut& buffer) override;
	// utility
	//virtual DeviceDef *  getDefinition();
protected:
	// Are these 'output' or 'cache' variables
	Pose::vec velocity;
	Pose::rot velocityAngular;
	
	// Moved from MoverDef
	float speedLinear;
	float speedAngular;
	float thrust,torque;	// force values for kinematic mode
	bool kinematic;
	bool autorotate;		// should we rotate automaticly
private:
	MoverCharacter* prototype;
};

// This cast will be exported to LUA
inline MoverCharacter * toCharacter(Device* mover)
{
	return dynamic_cast<MoverCharacter*>(mover);
}

} // namespace sim
