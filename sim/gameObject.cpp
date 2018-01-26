#include "gameObject.h"
#include "objectManager.h"

namespace sim
{
#ifdef FUCK_THIS
/////////////////////////////////////////////////////////////////////////
// GameObject basic definition
/////////////////////////////////////////////////////////////////////////
GameObjectDef::GameObjectDef(ObjectManager *store)
	:manager(store),population(0),localID(invalidID),body(store)
{
	attach();
}

GameObjectDef::GameObjectDef(const GameObjectDef &def)
	:manager(def.manager),population(0),name(def.name),body(def.body)
{
	attach();
	fxDie = def.fxDie;
	fxIdle = def.fxIdle;
}

GameObjectDef::~GameObjectDef()
{
//	LogFunction(*g_logger);
	assert(!population);	
	//if(body)delete body;
	detach();
}

_Scripter * GameObjectDef::getScripter()
{
	return manager->getScripter();
}

void GameObjectDef::addRef()
{
	population++;
}

void GameObjectDef::decRef()
{
	population--;
	assert(population>=0);
}
size_t GameObjectDef::getPopulation()const
{
	return population;
}

#endif
/////////////////////////////////////////////////////////////////////////
// Game object
/////////////////////////////////////////////////////////////////////////
GameObject::GameObject(ObjectManager *parent)
	:health(1.0f),onDamage(NULL), localID(invalidID), body(parent)
{
	collisionGroup = cgBody;
	player = 0;
	prototype = nullptr;
	manager = nullptr;
	maxHealth = 100;

	parent->registerObject(this);

	attachBody(body.create());
}

GameObject::~GameObject()
{
	if(manager)
		manager->removeObject(this);
}

ObjectManager * GameObject::getManager() 
{	
	return manager; 
}

ObjectType GameObject::getType() const 
{ 
	return typeVoid; 
}

Scripter * GameObject::getScripter()
{ 
	return manager ? manager->getScripter() : NULL; 
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
	if( effects )
		effects->update(dt);
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
