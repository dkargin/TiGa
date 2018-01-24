#pragma once
#include "../sim/gameObject.h"
#include "../sim/perception.h"

class Projectile;
class ProjectileDef;
class PerceptionDef;
class ObjectManager;

class ProjectileDef: public GameObjectDef
{
public:	
	enum ProjectileType
	{
		ptDirect,
		ptBeam,
		ptHoming,
	};
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

	Projectile * construct(IO::StreamIn * buffer);
	GameObject * create(IO::StreamIn * buffer);
protected:
	ProjectileDef(const ProjectileDef &def);
};
	
class Projectile: public GameObject, public PerceptionClient
{
	bool finished;
public:
	OBJECT_IMPL(Projectile,typeProjectile);
	//float velocity;
	float livingTime;
	Perception * perception;
	GameObjectPtr target;	// tracked target
	Unit *Source;
public:
	Projectile(ProjectileDef *def,Unit *Src=NULL);	
	~Projectile();

	bool IsHoming() const;
	bool IsBeam() const;
	bool IsDirect() const;
	virtual CollisionGroup getCollisionGroup()const
	{
		return cgProjectile;
	}
	// from PerceptionClient
	virtual bool addObject(GameObjectPtr object)
	{
		if(target != NULL && object->getType() == typeUnit && isHostile(object.get(),this))
			target = object;
		return true;
	}
	PerceptionClient * getPerceptionClient() { return this;}
	void onHit(GameObject * object);
	void update(float dt);
};


//class EffectDef: public GameObjectDef,virtual public LuaHelper<EffectDef>
//{
//public:		
//	Damage damage;
//	FxAnimation2 * animation;
//	//FxPointer impact;		// animation played on impact
//	//FxParticles * particles;
//	
//	EffectDef(ObjectManager & base);
//	//virtual ~ EffectDef(){}
//	virtual GameObject * create(IO::StreamIn *context);
//};
