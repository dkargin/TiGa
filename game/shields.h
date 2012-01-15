#ifndef _DEVICE_SHIELDS_H
#define _DEVICE_SHIELDS_H

class ObjectManager;

class GeneratorDef: public DeviceDef
{
public:
	GeneratorDef(ObjectManager &manager):DeviceDef(manager),maxStorage(0),generated(0){}
public:
	float maxStorage;	/// maximum energy storage
	float generated;	/// energy output
};

class Generator : public Device
{
public:
	GeneratorDef * definition;
	float storage;
	Generator(GeneratorDef *def)
		:Device(def),storage(0)
	{
	}
};
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

class ForceField : public Device
{
public:
	OBJECT_IMPL(ForceField,typeDummy);
	GameObject dome;	/// solid sphere for shield dome
	ForceFieldDef * definition;
	bool enabled;
	bool recharge;
public:
	ForceField(ForceFieldDef *def);
	enum FieldState
	{
		fsDisabled, /// manually turned off
	};
	void enableField();
	void update(float dt);
	int writeState(IO::StreamOut &buffer);
	int readState(IO::StreamIn &buffer);
	bool validCommand(int port,DeviceCmd cmd)const;	
};

#endif