#include "stdafx.h"
#include "gameObject.h"
#include "objectManager.h"

/////////////////////////////////////////////////////////////////////////
// SolidObject basic definition
/////////////////////////////////////////////////////////////////////////
SolidObject::SolidObject()
	:solid(NULL),pose(0,0,0)
{}

SolidObject::~SolidObject()
{
	detachBody();
}

bool SolidObject::isSolid()const
{
	return solid != NULL;
}

void SolidObject::detachBody()
{
	if(solid)
	{
		solid->GetWorld()->DestroyBody(solid);
		solid=NULL;
	}
}

void SolidObject::attachBody(b2Body * s)
{
	assert(s!=solid);
	detachBody();
	solid=s;
	solid->SetUserData(this);
}

const b2Body  * SolidObject::getBody() const
{
	return solid;
}

b2Body  * SolidObject::getBody()
{
	return solid;
}

const Pose::pos & SolidObject::getPosition() const
{
	return pose.getPosition();
}

Pose::vec SolidObject::getDirection() const
{
	return pose.getDirection();
}

void SolidObject::setPosition(const Pose::vec &pos)
{
	pose.setPosition(pos);
	updateSolidPose();  
}

void SolidObject::updateSolidPose()
{
	if(isSolid())
	{
		getBody()->SetTransform(b2conv(pose.position),pose.orientation);
	}
}

void SolidObject::setDirection(const Pose::vec2 &dir)
{
	pose.setDirection(dir);
	updateSolidPose();	
}

void SolidObject::setPose(const Pose &p)
{
	pose = p;
	updateSolidPose();	
}

Pose SolidObject::getPose() const
{
	return pose;
}

void SolidObject::syncPose()
{
	if(isSolid())
	{
		pose.setPosition(conv(getBody()->GetPosition()));
		pose.orientation = getBody()->GetAngle();
	}
}
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
	LogFunction(*g_logger);
	assert(!population);	
	//if(body)delete body;
	detach();
}
void GameObjectDef::attach()
{
	//if(manager)
	//	manager->ObjStore<GameObjectDef>::registerObject(this);
}
void GameObjectDef::detach()
{
	//if(manager)
	//	manager->ObjStore<GameObjectDef>::remove(this);
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
/////////////////////////////////////////////////////////////////////////
// Game object
/////////////////////////////////////////////////////////////////////////
GameObject::GameObject(ObjectManager *parent,GameObjectDef* def)
	:health(1.0f),onDamage(NULL),player(0),definition(def),localID(invalidID),collisionGroup(cgBody),effects(&parent->fxManager)
{
	attach();
	if(definition)
	{
		definition->addRef();	  
		if(!definition->body.isEmpty())
		{
			attachBody(definition->body.create());
		}
	}
}
GameObject::~GameObject()
{
	if(definition)
		definition->decRef();	
	
	detach();
}

void GameObject::attach()
{
	//if(definition && definition->manager)
	//	definition->manager->ObjStore<GameObject>::registerObject(this);
}

void GameObject::detach()
{
	//if(definition && definition->manager)
	//	definition->manager->ObjStore<GameObject>::remove(this);
}

bool GameObject::isUnique()const
{
	return definition ? definition->getPopulation() == 1 : false;
}

void GameObject::makeUnique()
{
	if(!isUnique() && definition)
	{
		definition->decRef();
		definition = definition->clone();
		definition->addRef();
	}
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
	effects.update(dt);
}
void GameObject::setCollisionGroup(CollisionGroup group)
{
	collisionGroup=group;
}

CollisionGroup GameObject::getCollisionGroup()const
{
	return collisionGroup;
}

int GameObject::writeState(IO::StreamOut &buffer)
{
	buffer.write(getPose());
	buffer.write(health);
	return 1;
}

int GameObject::readState(IO::StreamIn &buffer)
{
	Pose pose;
	buffer.read(pose);
	getBody()->SetTransform(b2conv(pose.position),pose.orientation);
	buffer.read(health);
	return 1;
}

Sphere2 GameObject::getBoundingSphere()const
{
	return definition->body.getBoundingSphere();
}

AABB2 GameObject::getOOBB()const			// get object oriented bounding box
{
	return definition->body.getOOBB();
}

void GameObject::damage(const Damage &dmg)
{		
	if(onDamage!=0)
		onDamage->onDamage(dmg);
	health-=dmg;//.Value[0];
	//health-=dmg[1];//.Value[1];
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

void GameObject::save(IO::StreamOut & stream)
{
	writeState(stream);
}

void GameObject::load(IO::StreamIn & stream)
{
	readState(stream);
}

///////////////////////////////////////////////////
b2Body * createSolidBox(ObjectManager *m,float width,float height,float mass)
{
	LogFunction(*g_logger);
	b2BodyDef bodyDef;
	bodyDef.type=mass>0.0f?b2_dynamicBody:b2_kinematicBody;
	bodyDef.position.Set(0.0f, 0.0f);
	b2Body *body=m->getDynamics()->CreateBody(&bodyDef);
	b2PolygonShape shape;
	shape.SetAsBox(width, height);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = mass;
	fixtureDef.friction = 0.0f;
	body->CreateFixture(&fixtureDef);
	return body;
}

b2Body * createSolidSphere(ObjectManager *m,float D,float mass)
{
	LogFunction(*g_logger);
	b2BodyDef bodyDef;
	bodyDef.type=mass>0.0f?b2_dynamicBody:b2_kinematicBody;
	bodyDef.bullet=true;
	bodyDef.position.Set(0.0f, 0.0f);
	b2Body *body=m->getDynamics()->CreateBody(&bodyDef);
	b2CircleShape shape;
	shape.m_radius=D;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = mass;
	fixtureDef.friction = 0.0f;
	body->CreateFixture(&fixtureDef);
	return body;
}

int randRange(int min,int max)
{
	return min+rand()%abs(max-min);
}

///////////////////////////////////////////////////////
void PolyDesc::addPoint(float x,float y)
{
	return addPoint(vec2f(x,y));
}

void PolyDesc::addPoint(const vec2f & v)
{
	points.push_back(v);
}

///////////////////////////////////////////////////////
BodyDesc::BodyDesc(ObjectManager *manager)
	:manager(manager),bounds(true),boundSphere(vec2f(0,0),0),boundBox(vec2f(0,0),vec2f(0,0))
{
	frozen = false;
	type = b2_dynamicBody;
}

/// for cloning
BodyDesc::BodyDesc(const BodyDesc &desc)
	:manager(desc.manager),b2BodyDef(desc),frozen(desc.frozen), bounds(desc.bounds), boundBox(desc.boundBox), boundSphere(desc.boundSphere)
{  
	assign(desc);
}

BodyDesc::~BodyDesc(){}

BodyDesc & BodyDesc::operator=(const BodyDesc & desc)
{
	assert(this != &desc);
	assign(desc);
	return *this;
}
/// assing b2BodyDef structure
void AssignBody(b2BodyDef & target,const b2BodyDef & source)
{
	target = source;
}
void BodyDesc::assign(const BodyDesc & desc)
{
	AssignBody(*this,desc);
	allocator.Clear();
	manager = desc.manager;
	frozen = desc.frozen;
	bounds = desc.bounds;
	boundBox = desc.boundBox;
	boundSphere = desc.boundSphere;
	fixtures.resize(desc.fixtures.size());
	for(int i = 0; i < fixtures.size(); ++i)
	{
		fixtures[i] = desc.fixtures[i];
		if(desc.fixtures[i].shape)
		{
			fixtures[i].shape = desc.fixtures[i].shape->Clone(&allocator);
		}
	}
}

void BodyDesc::updateBounds()
{
	if(bounds)
		return;
	b2Transform tr;
	tr.SetIdentity();
	boundBox = AABB2(vec2f(0.f),vec2f(0.f));
	for(auto it = fixtures.begin(); it != fixtures.end(); ++it)
	{		
		b2FixtureDef & fixture = *it;
		b2AABB aabb;
		fixture.shape->ComputeAABB(&aabb,tr,0);
		boundBox &= AABB2::minmax(conv(aabb.lowerBound),conv(aabb.upperBound));
	}
	boundSphere.center = boundBox.center;
	boundSphere.radius = boundBox.dimensions.length();
	bounds = true;
}
const AABB2 & BodyDesc::getOOBB()
{
	updateBounds();
	return boundBox;
}
const Sphere2 & BodyDesc::getBoundingSphere()
{
	updateBounds();
	return boundSphere;
}
bool BodyDesc::isEmpty()const
{
	return fixtures.empty();
}
/// create body from stored fixture data
b2Body * BodyDesc::create()
{
	updateBounds();
	if(isEmpty())
		return NULL;
	b2Body *body = manager->getDynamics()->CreateBody(this);
	for(auto it = fixtures.begin(); it != fixtures.end(); ++it)
		body->CreateFixture(&*it);
	body->ResetMassData();
	float mass = body->GetMass();
	if(frozen)
		body->SetType(b2BodyType::b2_kinematicBody);  
	return body;
}

b2FixtureDef & BodyDesc::addFixture(float density, b2Shape & shape)
{
	bounds = false;
	fixtures.push_back(FixtureDef());
	FixtureDef & result = fixtures.back();
	result.shape = shape.Clone(&allocator);
	result.density = density;
	return fixtures.back();
}

b2FixtureDef & BodyDesc::addBox(float width,float height,float mass)
{		
	b2PolygonShape shape;	
	shape.SetAsBox(width, height);
	return addFixture(mass,shape);
}

b2FixtureDef & BodyDesc::addSphere(float radius,float mass)
{
	b2CircleShape shape;
	shape.m_radius = radius;
	b2FixtureDef & result = addFixture(mass,shape);
	/// manually update object bounds
	if(fixtures.size() == 1)
	{
		boundBox.center = Pose::vec::zero();
		boundBox.dimensions = Pose::vec(radius,radius);
		boundSphere.center = Pose::vec::zero();
		boundSphere.radius = radius;
		bounds = true;
	}
	return result;
}

b2FixtureDef & BodyDesc::addPolygon(const PolyDesc & desc, float mass)
{
	b2PolygonShape shape;
	shape.Set((b2Vec2*)&desc.points.front(),desc.points.size());

	return addFixture(mass,shape);
}

////////////////////////////////////////////////////////////////////////
void DestroyObject(GameObject * object)
{
	delete object;
}