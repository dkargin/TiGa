#pragma once

namespace sim
{
class WeaponProjectile;

class WeaponProjectile: public WeaponProjecdtilePair::Obj
{
public:
	WeaponProjectile(WeaponProjectileDef *def,Device::BuildContext *context);
	~WeaponProjectile();
	int execute_Action(int port);
	int writeSync(IOBuffer &buffer);
	int readSync(IOBuffer &buffer);
};

#ifdef FUCK_THIS
class WeaponProjectileDef: public WeaponProjecdtilePair::Def
{
public:
	ProjectileDef *projectile;
	WeaponProjectileDef(DeviceManager *manager):Definition(manager),projectile(NULL){}	
};
#endif

} // endif
