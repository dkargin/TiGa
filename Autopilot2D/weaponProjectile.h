#pragma once

class WeaponProjectile;
class WeaponProjectileDef;
typedef DevicePair<WeaponProjectileDef,WeaponProjectile,WeaponPair> WeaponProjecdtilePair;

class WeaponProjectile: public WeaponProjecdtilePair::Obj
{
public:
	WeaponProjectile(WeaponProjectileDef *def,Device::BuildContext *context);
	~WeaponProjectile();
	int execute_Action(int port);
	int writeSync(IOBuffer &buffer);
	int readSync(IOBuffer &buffer);
};

class WeaponProjectileDef: public WeaponProjecdtilePair::Def
{
public:
	ProjectileDef *projectile;
	WeaponProjectileDef(DeviceManager *manager):Definition(manager),projectile(NULL){}	
};