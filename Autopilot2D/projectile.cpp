#include "stdafx.h"
#include "objectManager.h"
#include "projectile.h"
#include "perception.h"
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
///////////////////////////////////////////////////////////////////
// Projectile - simple moving object
///////////////////////////////////////////////////////////////////
Projectile::Projectile(ProjectileDef *def,Unit *Src)
:GameObject(def->manager,def),Source(Src),finished(false),livingTime(0),perception(NULL)
{
	if(def->fxIdle)
	{
		FxPointer p = def->fxIdle->clone();
		p->start();
		effects.attach(p);
	}
	if(def->perception)
	{
		perception = new Perception(def->perception);
		perception->unit = this;
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
			vec2f dir = (pos - pose.getPosition()).normalise();
			setDirection(dir);			
			body->SetLinearVelocity(b2conv(dir*localDef().velocity));
		}
	}

	
	effects.update(dt);
	
	//getBody()->ApplyForce(b2Vec2(40000,0),getBody()->GetPosition());
	if(/*IsDirect() && */!finished)
	{		
		body->SetLinearVelocity(b2conv(getDirection()*localDef().velocity));
		//solid->addForce(getDirection()*definition->velocity,forceModeSmoothVelocityChange);
		finished = true;
	}

	// selfdestruct when living time ends
	livingTime+=dt;	
	if(livingTime > localDef().livingTime)
		this->kill();	
}

void Projectile::onHit(GameObject *object)
{
	object->damage(localDef().damage);
	kill();
	if(localDef().impact)
	{
		auto obj = localDef().impact->clone();
		obj->setPose(getPose());
		obj->start();
		manager()->fxManager.pyro.runEffect(obj);
	}
}