#pragma once

#include "gameObject.h"
#include "perception.h"

namespace sim
{

class Projectile;
class ObjectManager;

enum class ProjectileType
{
	ptDirect,
	ptBeam,
	ptHoming,
};

#ifdef FUCK_THIS
class ProjectileDef: public GameObjectDef
{
public:
	int projectileType;
	float velocity;
	float livingTime;
	float size;				// object size
	float turning;			// angular velocity
	Damage damage;
	FxPointer impact;		// animation played on impact	
	PerceptionDef * perception;

	OBJECT_DEF(Projectile,typeProjectile);

	ProjectileDef(ObjectManager * base);
	virtual ~ ProjectileDef(){}

	Projectile * construct(StreamIn * buffer);
	GameObject * create(StreamIn * buffer);
protected:
	ProjectileDef(const ProjectileDef &def);
};
#endif
	
class Projectile:
		public GameObject,
		public PerceptionClient
{
	bool finished;
public:
	//OBJECT_IMPL(Projectile,typeProjectile);
	//float velocity;
	float livingTime;
	// Why does projectile has 'perception'.
	// Because it could be a missile?
	Perception * perception;
	GameObjectPtr target;	// tracked target
	Unit *Source;

	// From definition
	ProjectileType projectileType;
	float velocity;					//< Velocity limit
	float livingTimeLimit;	//< Life time limit
	float size;							//< Collision size
	float turning;					//< Angular velocity limit for homing missiles
	Damage damage;
	Fx::EntityPtr impact;		// animation played on impact
	//PerceptionDef * perception;
public:
	Projectile(Projectile* def,Unit* Src=nullptr);
	~Projectile();

	bool IsHoming() const;
	bool IsBeam() const;
	bool IsDirect() const;
	virtual CollisionGroup getCollisionGroup()const
	{
		return cgProjectile;
	}

	// from PerceptionClient
	virtual bool addObject(GameObjectPtr object) override
	{
		if(target != NULL && object->getType() == typeUnit && isHostile(object.get(),this))
			target = object;
		return true;
	}
	PerceptionClient * getPerceptionClient() { return this;}
	void onHit(GameObject * object);
	void update(float dt);
};

} // namespace sim
