/*
 * solidobject.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: vrobot
 */


#include "solidobject.h"
#include "objectManager.h"

namespace sim
{
///////////////////////////////////////////////////
b2Body * createSolidBox(ObjectManager *m,float width,float height,float mass)
{
//	LogFunction(*g_logger);
	b2BodyDef bodyDef;
	bodyDef.type=mass>0.0f?b2_dynamicBody:b2_kinematicBody;
	bodyDef.position.Set(0.0f, 0.0f);
	b2Body* body = m->getDynamics()->CreateBody(&bodyDef);
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
//	LogFunction(*g_logger);
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
BodyDesc::BodyDesc(ObjectManager* manager)
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

void BodyDesc::updateBounds() const
{
	if(bounds)
		return;
	b2Transform tr;
	tr.SetIdentity();
	boundBox = AABB2(vec2f(0.f),vec2f(0.f));
	for(const b2FixtureDef& fixture: fixtures)
	{
		b2AABB aabb;
		fixture.shape->ComputeAABB(&aabb,tr,0);
		boundBox &= AABB2::minmax(conv(aabb.lowerBound),conv(aabb.upperBound));
	}
	boundSphere.center = boundBox.center;
	boundSphere.radius = boundBox.dimensions.length();
	bounds = true;
}

const AABB2 & BodyDesc::getOOBB() const
{
	updateBounds();
	return boundBox;
}

const Sphere2 & BodyDesc::getBoundingSphere() const
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
	b2Body* body = manager->getDynamics()->CreateBody(this);
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

/////////////////////////////////////////////////////////////////////////
// SolidObject basic definition
/////////////////////////////////////////////////////////////////////////
SolidObject::SolidObject()
	:solid(nullptr), pose(0,0,0)
{}

SolidObject::~SolidObject()
{
	detachBody();
}

bool SolidObject::isSolid()const
{
	return solid != nullptr;
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

void SolidObject::setDirection(const Pose::vec &dir)
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

Pose::vec SolidObject::getVelocityLinear() const
{
	const b2Body * body = getBody();
	if(body)
	{
		return conv(body->GetLinearVelocity());
	}
	else
		return Pose::vec::zero();
}

Pose::vec3 SolidObject::getVelocityLinear3() const
{
	const b2Body * body = getBody();
	if(body)
	{
		vec2f velocity2 = conv(body->GetLinearVelocity());
		return Pose::vec3(velocity2[0], velocity2[1], 0);
	}
	else
		return Pose::vec3::zero();
}

Sphere2 SolidObject::getBoundingSphere() const
{
	// TODO: implement
	return Sphere2(getPosition(), 0.f);
}

AABB2 SolidObject::getOOBB()const			// get object oriented bounding box
{
	// TODO: implement
	return AABB2(getPosition(), vec2f::zero());
}

}
