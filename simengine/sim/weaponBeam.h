#pragma once

namespace sim
{

class WeaponBeam;

class WeaponBeam: public Weapon
{
public:
	bool fire;
	WeaponBeam(WeaponBeam* def, Device::BuildContext *context);
	~WeaponBeam();

	FxBeam * beam;
	void update(float dt);
	int execute_Action(int port);
	int writeSync(IOBuffer &buffer);
	int readSync(IOBuffer &buffer);
};

#ifdef FUCK_THIS
class WeaponBeamDef: public WeaponBeamPair::Def
{
public:
	float range;	// max range
	float beamWidth;
	float beamLife;	// pulse length
	FxBeam * beam;
	WeaponBeamDef(DeviceManager *manager):Definition(manager),beam(NULL){}	
};
#endif

}
