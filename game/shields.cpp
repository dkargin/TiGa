#include "shields.h"

#include "../sim/device.h"
#include "../sim/objectManager.h"
#include "../sim/unit.h"

////////////////////////////////////////////////////////////////
// ForceFieldDef implementation
////////////////////////////////////////////////////////////////

ForceFieldDef::ForceFieldDef(DeviceManager& manager)
	:DeviceDef(manager),size(100),amount(100)
{}	

Device *ForceFieldDef::create(IO::StreamIn *context)
{
	return new ForceField(this);
}

////////////////////////////////////////////////////////////////
// ForceField implementation
////////////////////////////////////////////////////////////////

ForceField::ForceField(ForceFieldDef *def)
	:definition(def),Device(def),dome(NULL,NULL),enabled(false),recharge(false)
{}

void ForceField::enableField()
{
	if(!enabled)
	{
		GameObject * object = &dome;
		object->setPlayer(getMaster()->getPlayer());
		object->attachBody(createSolidSphere(&definition->manager,definition->size,0.0000));
		recharge = false;
		enabled = true;
	}
}

void ForceField::update(float dt)
{
	enableField();
	dome.setPose(getGlobalPose());
	dome.update(dt);
}

int ForceField::writeState(IO::StreamOut &buffer){return 0;}
int ForceField::readState(IO::StreamIn &buffer){return 0;}
bool ForceField::validCommand(int port,DeviceCmd cmd)const {return false;}
