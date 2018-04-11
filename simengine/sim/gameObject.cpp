#include "gameObject.h"
#include "objectManager.h"

namespace sim
{

GameObject::GameObject(GameObject* base)
	:health(1.0f),onDamage(NULL), localID(invalidID), body(scene)
{
	collisionGroup = cgBody;
	player = 0;
	prototype = base;
	this->scene = scene;
	maxHealth = 100;

	attachBody(body.create());
}

GameObject::~GameObject()
{
	if(scene)
		scene->removeObject(shared_from_this());
}

ObjectManager * GameObject::getManager() 
{	
	return scene; 
}

ObjectType GameObject::getType() const 
{ 
	return typeVoid; 
}

Fx::EntityPtr GameObject::getGraphics()
{
	return fx_root.sharedMe();
}

Scripter * GameObject::getScripter()
{ 
	return scene ? scene->getScripter() : NULL; 
}

bool GameObject::isUnique()const
{
	return prototype != nullptr;
}


float GameObject::getHealth() const
{
	return health;
}

int GameObject::getPlayer() const
{
	return player;	// return default neutral player
}

void GameObject::setPlayer(int p)
{
	player = p;
}


void GameObject::update(float dt)
{
	syncPose();
	fx_root.update(dt);
}

void GameObject::setCollisionGroup(CollisionGroup group)
{
	collisionGroup = group;
}

CollisionGroup GameObject::getCollisionGroup()const
{
	return collisionGroup;
}

int GameObject::writeState(StreamOut &buffer)
{
	buffer.write(getPose());
	buffer.write(health);
	return 1;
}

int GameObject::readState(StreamIn &buffer)
{
	Pose pose;
	buffer.read(pose);
	getBody()->SetTransform(b2conv(pose.position),pose.orientation);
	buffer.read(health);
	return 1;
}

Sphere2 GameObject::getBoundingSphere() const
{
	return body.getBoundingSphere();
}

AABB2 GameObject::getOOBB()const			// get object oriented bounding box
{
	return body.getOOBB();
}

void GameObject::damage(const Damage &dmg)
{		
	if(onDamage!=0)
		onDamage->onDamage(dmg);
	health-=dmg;
}

bool GameObject::isDead()const
{
	return health<=0.0f;
}

void GameObject::kill()
{
	if(onDamage!=0)
	{
		Damage dmg(health);
		onDamage->onDamage(dmg);
	}
	health=0;
}

void GameObject::save(StreamOut & stream)
{
	writeState(stream);
}

void GameObject::load(StreamIn & stream)
{
	readState(stream);
}


////////////////////////////////////////////////////////////////////////
void DestroyObject(GameObject * object)
{
	delete object;
}

} // namespace sim
