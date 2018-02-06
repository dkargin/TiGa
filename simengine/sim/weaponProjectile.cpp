#include "weaponProjectile.h"
#include "device.h"
#include "projectile.h"
#include "weapon.h"

namespace sim
{
WeaponProjectile::WeaponProjectile(WeaponProjectileDef *def,Device::BuildContext *context)
	:Object(def,context)
{}

WeaponProjectile::~WeaponProjectile()
{}

int WeaponProjectile::execute_Action(int port)
{
	if(port!=portShoot)return 0;
	
	if(time<=0)
	{
		if(fxShoot)	fxShoot->start();	
		
		if(definition->projectile)
		{
			GameObject *object = definition->projectile->create(NULL);
			if(object)				
				object->setPose(getMuzzlePose());					
		}		
		time=definition->delay;
	}
	return 1;	
}

int WeaponProjectile::writeSync(IOBuffer &buffer)
{
	return buffer.write(time);
}
int WeaponProjectile::readSync(IOBuffer &buffer)
{
	return buffer.read(time);
}
}
