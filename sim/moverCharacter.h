#ifndef MOVER_CHARACTER_INCLUDED
#define MOVER_CHARACTER_INCLUDED
#include "mover.h"

/////////////////////////////////////////////////////////////
// ��������� characterController ��� ��� ������
// Re: ��� �� ���������.
/////////////////////////////////////////////////////////////
struct MoverCharacterDef: public MoverDef
{
	//float size;
	float speedLinear;
	float speedAngular;
	float thrust,torque;	// force values for kinematic mode
	bool kinematic;
	bool autorotate;		// should we rotate automaticly
	//vec3 dimensions;		// bounds dimensions
	MoverCharacterDef(DeviceManager &parent);
	virtual Device * create(IO::StreamIn *context);
private:
	void build();
	bool ready;
};

class MoverCharacter: public Mover
{
public:	
	MoverCharacter(MoverCharacterDef* def);
	~MoverCharacter();

	virtual void update(float dt);	

	virtual void setVelocity(float x,float y,float z,float vel);
	virtual void setVelocity(const Pose::vec &dir,float vel);
	virtual void directionControl(const Pose::vec& direction, float speed);
	bool validCommand(int port,DeviceCmd cmd)const;

	virtual float getMaxVelocity(int dir) const { return dir? definition->speedLinear : definition->speedAngular; }

	// sync control
	//int readDesc(IOBuffer &context);
	//int writeDesc(IOBuffer &context);	
	int readState(IO::StreamIn &buffer);
	int writeState(IO::StreamOut &buffer);
	// utility
	virtual DeviceDef *  getDefinition();	
protected:
	Pose::vec velocity;			// current velocity
	Pose::rot velocityAngular;
	
	MoverCharacterDef* definition;
};

inline MoverCharacter * toCharacter(Device * mover)
{
	return dynamic_cast<MoverCharacter*>(mover);
}

#endif
