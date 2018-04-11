#ifndef _DEVICE_SHIELDS_H
#define _DEVICE_SHIELDS_H


#include <simengine/sim/device.h>

class ObjectManager;

/*
class GeneratorDef: public DeviceDef
{
public:
	GeneratorDef(ObjectManager &manager):DeviceDef(manager),maxStorage(0),generated(0){}
public:
	float maxStorage;	/// maximum energy storage
	float generated;	/// energy output
};
*/

class Generator : public sim::Device
{
public:

	float storage;
	// From Def
	float maxStorage = 0;	/// maximum energy storage
	float generated = 0;	/// energy output

	Generator(Generator* proto = nullptr)
		:Device(proto),storage(0)
	{
	}

	Generator* definition;
};

/*
class ForceField;

class ForceFieldDef : public DeviceDef
{
public:
	ForceFieldDef(DeviceManager& manager);
	Device *create(IO::StreamIn *context=NULL);
	FxPointer impact;	/// when someone hits
	bool hitProjectiles;
	bool hitObjects;
	bool reflectBeams;
	float amount;
	float integrity;
	float energyConsumption;
	float size;			/// dome radius
};
*/
class ForceField : public sim::Device
{
public:
	sim::GameObject dome;	/// solid sphere for shield dome
	bool enabled = false;
	bool recharge = false;
	// From Def

	bool hitProjectiles;
	bool hitObjects;
	bool reflectBeams;
	float amount;
	float integrity;
	float energyConsumption;
	float size;			/// dome radius
public:

	ForceField(ForceField *proto = nullptr);

	enum FieldState
	{
		fsDisabled, /// manually turned off
	};

	void enableField();
	void update(float dt);

	int writeState(sim::StreamOut &buffer);
	int readState(sim::StreamIn &buffer);
	bool validCommand(int port, sim::DeviceCmd cmd)const;

private:
	ForceField* definition = nullptr;
};

#endif
