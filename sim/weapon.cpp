#include "weapon.h"
#include "device.h"
#include "projectile.h"
#include "unit.h"

namespace sim
{

#ifdef FUCK_THIS
WeaponDef::WeaponDef(DeviceManager &m):DeviceDef(m),/*beam(NULL),*/projectile(NULL)/*weaponType(weaponProjectile),*/
{
	//beamWidth = 1;	
	invokerAction(0,HGEK_SPACE);
}

WeaponDef::~WeaponDef()
{}

Device * WeaponDef::create(IO::StreamIn *context)
{
	return new Weapon(this,context);
}

float WeaponDef::getEffectiveRange()const
{
	//if(weaponType == WeaponDef::weaponBeam)
	//	return maxRange;
	//else if(weaponType == WeaponDef::weaponProjectile)
	return projectile->velocity * projectile->livingTime;
	//return 0;
}

#endif
///////////////////////////////////////////////////////
// Basic weapon
///////////////////////////////////////////////////////
Weapon::Weapon(Weapon* def,StreamIn * buffer)
:Device(def)
{
	time = 0;
	fire = false;/*,beam(NULL)*/
	weaponState = stateReady;
	ammo = 0;
	burstLeft = 0;

	if(def)
	{
		memcpy(&weaponData, &def->weaponData,sizeof(weaponData));
		ammo = def->ammo;
		if(def->fxShoot)
		{
			fxShoot = def->fxShoot->clone();
			fx_root->attach(fxShoot);
		}
	}
}

Weapon::~Weapon()
{}

float Weapon::getEffectiveRange()const
{
	//return weaponData.velocity * weaponData.maxRange;
	return weaponData.maxRange;
}
/*
int Weapon::writeDesc(IOBuffer &context)
{
	context.write(timeShootDelay);
	context.write(timeShootDuration);
	context.write(timeReload);
	context.write(muzzleOffset);
	context.write(maxAmmo);
	context.write(beamWidth);
	context.write(maxRange);
	return 1;
}
int Weapon::readDesc(IOBuffer &context)
{
	context.read(timeShootDelay);
	context.read(timeShootDuration);
	context.read(timeReload);
	context.read(muzzleOffset);
	context.read(maxAmmo);
	context.read(beamWidth);
	context.read(maxRange);
	return 1;
}
*/
float frand(float range)
{
	return (rand()/(float)RAND_MAX-0.5)*range;
}

void Weapon::shoot()
{
	this->query_Action(portShoot);
}

void Weapon::doShoot(float dt)
{
	Pose spread(0,0,frand(weaponData.spread)*M_PI/180);
	// TODO: Fix projectile spawning
	throw std::runtime_error("Not implemented");
	/*
	if( weaponType==weaponProjectile)
	{
		/// this conversion seems to be bad. Maybe better to use definition->construct ?
		Projectile *object = dynamic_cast<Projectile*>(projectile->create(NULL));
		assert(object);

		object->Source = getMaster();	/// let ignore out unit for collisions
		object->player = getMaster()->getPlayer();

		if(object)object->setPose(getMuzzlePose()*spread);			
	}*/
}

void Weapon::updateBurst(float dt)
{
	/*if(definition->weaponType==WeaponDef::weaponBeam && beam)
	{		
		static RayHits hits;
		hits.reserve(255);
		hits.resize(0);
		float length=definition->maxRange;
		Pose pose=getMuzzlePose();
		getManager()->objectManager->raycast(pose,length,hits);							

		for(auto it=hits.begin();it<hits.end();++it)
			if(it->object!=master->getUnit())
			{
				length=vecLength(pose.position-hits.front().point);
				break;
			}
		Pose spread(0,0,frand(definition->spread)*M_PI/180);
		beam->setPose(spread * Pose(definition->muzzleOffset,0,0));
		beam->setLength(length);
	}*/
	time -= dt;
	while(burstLeft && time < 0.f)
	{
		time += weaponData.burstDelay;
		burstLeft --;
		doShoot(dt);		
	}
	if(!burstLeft && time <0.f)
	{
		/*if(beam)
		{
			beam->stop();
			beam->show(false);
		}*/
		time += weaponData.timeShootDelay;
		weaponState = stateDelay;			
	}
}

void Weapon::update(float dt)
{
	Device::update(dt);

	fx_root->setPose(pose);
	fx_root->update(dt);

	if(weaponState == stateBurst)
		updateBurst(dt);
	else if (weaponState == stateDelay)
	{
		time -= dt;
		if(time < 0.f)
			weaponState = stateReady;
	}
}

bool Weapon::validCommand(int port,DeviceCmd cmd)const
{
	if(port==portShoot && cmd==dcmdAction)return true;
	return false;
}

Pose Weapon::getMuzzlePose()
{
	return getGlobalPose()*Pose(weaponData.muzzleOffset,0,0);
}

int Weapon::execute_Action(int port)
{
	if(port!=portShoot)return 0;
	
	if(weaponState == stateReady)
	{
		if(fxShoot)	fxShoot->start();

		/*
		Pose spread(0,0,frand(definition->spread)*M_PI/180);

		
		if(definition->weaponType==WeaponDef::weaponBeam && beam)
		{			
			beam->setPose(spread * Pose(definition->muzzleOffset,0,0));
			beam->setLength(definition->maxRange);
			beam->show(true);	
			beam->start();
		}
		else if(definition->weaponType==WeaponDef::weaponProjectile && definition->projectile)
		{
			GameObject *object=definition->projectile->create(NULL);
			
			if(object)object->setPose(getMuzzlePose()*spread);			
		}*/
		time = weaponData.burstDelay;
		burstLeft = weaponData.burstSize;
		weaponState = stateBurst;
	}
	return 1;	
}

int Weapon::writeState(StreamOut &buffer)
{
	buffer.write(fire);
	buffer.write(time);
	buffer.write(ammo);
	return 1;
}

int Weapon::readState(StreamIn &buffer)
{
	buffer.read(fire);
	buffer.read(time);
	buffer.read(ammo);
	return 1;
}

} // namespace sim
