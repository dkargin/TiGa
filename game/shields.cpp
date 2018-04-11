#include "shields.h"

#include <simengine/sim/objectManager.h>
#include <simengine/sim/unit.h>

////////////////////////////////////////////////////////////////
// ForceFieldDef implementation
////////////////////////////////////////////////////////////////

/*
ForceFieldDef::ForceFieldDef(DeviceManager& manager)
	:DeviceDef(manager),size(100),amount(100)
{}

Device *ForceFieldDef::create(IO::StreamIn *context)
{
	return new ForceField(this);
}
*/
////////////////////////////////////////////////////////////////
// ForceField implementation
////////////////////////////////////////////////////////////////

ForceField::ForceField(ForceField *proto)
	:Device(proto), dome(nullptr)
{
	definition = proto;
}

void ForceField::enableField()
{
	if(!enabled)
	{
#ifdef FIX_THIS
		// Creation-level commands should be moved to a place, where world and objectManager is available
		sim::GameObject* object = &dome;
		object->setPlayer(getMaster()->getPlayer());

		auto body = sim::createSolidSphere(&definition->manager,definition->size,0.0000);
		object->attachBody();
		recharge = false;
		enabled = true;
#endif
	}
}

void ForceField::update(float dt)
{
	enableField();
	dome.setPose(getGlobalPose());
	dome.update(dt);
}

int ForceField::writeState(sim::StreamOut &buffer){return 0;}
int ForceField::readState(sim::StreamIn &buffer){return 0;}
bool ForceField::validCommand(int port, sim::DeviceCmd cmd)const {return false;}
