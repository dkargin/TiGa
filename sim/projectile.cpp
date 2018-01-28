#include "projectile.h"

#include <fxmanager.h>

#include "objectManager.h"
#include "perception.h"

namespace sim
{
#ifdef FUCK_THIS
///////////////////////////////////////////////////////////////////
// Projectile - simple moving object
///////////////////////////////////////////////////////////////////
ProjectileDef::ProjectileDef(ObjectManager * base)
:GameObjectDef(base), perception(NULL)
{
	velocity = 30;
	damage = 0;//Damage::zero();
	projectileType = ptDirect;
	livingTime = 1.0;
}

ProjectileDef::ProjectileDef(const ProjectileDef &def)
	:GameObjectDef(def), velocity(def.velocity), projectileType (def.projectileType)
{}

GameObject * ProjectileDef::create(IO::StreamIn * buffer)
{
	return construct(buffer);
}

Projectile * ProjectileDef::construct(IO::StreamIn *context)
{
	b2Body * solid = NULL;
	
	b2BodyDef bodyDef;
	bodyDef.type=b2_dynamicBody;
	bodyDef.bullet=true;
	bodyDef.position.Set(0.0f, 0.0f);
	solid=manager->getDynamics()->CreateBody(&bodyDef);
	//b2CircleShape shape;
	//shape.m_radius=1;

	b2PolygonShape shape;
	shape.SetAsBox(0.1f, 0.01f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	solid->CreateFixture(&fixtureDef);
	float mass = solid->GetMass();
	Projectile *result=NULL;
	if(solid)
	{
		result = new Projectile(this);
		result->attachBody(solid);
	}	
	return result;
}
#endif
///////////////////////////////////////////////////////////////////
// Projectile - simple moving object
///////////////////////////////////////////////////////////////////
Projectile::Projectile(Projectile *def, Unit *Src)
:GameObject(nullptr), Source(Src)
{
	finished = false;
	livingTime = 0;
	perception = nullptr;
	turning = false;
	size = 0.1;
	velocity = 1.0;
	projectileType = ProjectileType::ptDirect;
	livingTimeLimit = 1.0;
	damage = 0.0;

	if(def)
	{
		if(def->fxIdle)
		{
			fxIdle = def->fxIdle->clone();
		}

		if(def->perception)
		{
			perception = new Perception(def->perception);
			perception->unit = this;
		}
	}

	if(fxIdle)
	{
		fx_root.attach(fxIdle);
		fx_root.start();
	}
}

Projectile::~Projectile()
{
	if(perception)
		delete perception;
}

bool Projectile::IsHoming() const
{
	return perception != NULL;
}

bool Projectile::IsDirect() const
{
	return perception == NULL;
}

bool Projectile::IsBeam() const
{
	return false;
}

void Projectile::update(float dt)
{
	b2Body * body = getBody();
	GameObject::update(dt);	
	if(IsHoming())
	{
		if(!target)
			perception->update(dt);
		else
		{
			vec2f pos = target->getPosition();
			vec2f dir = (pos - pose.getPosition()).normalize();
			setDirection(dir);			
			body->SetLinearVelocity(b2conv(dir*velocity));
		}
	}

	fx_root.update(dt);
	
	//getBody()->ApplyForce(b2Vec2(40000,0),getBody()->GetPosition());
	if(/*IsDirect() && */!finished)
	{		
		body->SetLinearVelocity(b2conv(getDirection()*velocity));
		//solid->addForce(getDirection()*definition->velocity,forceModeSmoothVelocityChange);
		finished = true;
	}

	// selfdestruct when living time ends
	livingTime+=dt;

	if(livingTime >= livingTimeLimit)
		this->kill();	
}

void Projectile::onHit(GameObject *object)
{
	object->damage(this->damage);
	kill();

	// Show impact effect
	if(impact)
	{
		Fx::EntityPtr obj = impact->clone();
		obj->setPose(getPose());
		obj->start();

		// TODO: move effects to pyro.
		getManager()->fxManager->pyro.runEffect(obj);
	}
}
} // namespace sim
