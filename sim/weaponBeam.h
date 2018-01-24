#pragma once

class WeaponBeam;
class WeaponBeamDef;
typedef DevicePair<WeaponBeamDef,WeaponBeam,WeaponPair> WeaponBeamPair;

class WeaponBeam: public WeaponBeamPair::Obj
{
public:
	bool fire;
	WeaponBeam(WeaponBeamDef *def,Device::BuildContext *context);
	~WeaponBeam();
	FxBeam * beam;
	void update(float dt);
	int execute_Action(int port);
	int writeSync(IOBuffer &buffer);
	int readSync(IOBuffer &buffer);
};

class WeaponBeamDef: public WeaponBeamPair::Def
{
public:
	float range;	// max range
	float beamWidth;
	float beamLife;	// pulse length
	FxBeam * beam;
	WeaponBeamDef(DeviceManager *manager):Definition(manager),beam(NULL){}	
};