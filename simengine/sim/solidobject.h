#pragma once

#include "basetypes.h"
#include <Box2D/Box2D.h>

namespace sim
{

class ObjectManager;	//< TODO: I object!

inline const vec2f & conv(const b2Vec2 &v)
{
	return *reinterpret_cast<const vec2f*>(&v);
}
inline const b2Vec2 &b2conv(const vec2f &v)
{
	return *reinterpret_cast<const b2Vec2*>(&v);
}

inline const b2Vec2 &b2conv(const vec3f &v)
{
	return *reinterpret_cast<const b2Vec2*>(&v);
}

/// Polygon description for box2d body. Only maximum of 8 vertices is permitted
struct PolyDesc
{
	std::vector<vec2f> points;

	void addPoint(float x,float y);
	void addPoint(const vec2f & point);
};

/// description for box2d body
class BodyDesc: public b2BodyDef
{
public:
	typedef b2FixtureDef FixtureDef;
	std::vector<FixtureDef> fixtures;
	b2BlockAllocator allocator;
	ObjectManager * manager;
	bool frozen;

	BodyDesc(ObjectManager* manager);
	~BodyDesc();
	BodyDesc(const BodyDesc& desc);

	b2Body* create();
	float mass()const;
	bool isEmpty()const;  /// if result body is empty

	b2FixtureDef& addBox(float width,float height,float density);
	b2FixtureDef& addSphere(float size,float density);
	b2FixtureDef& addPolygon(const PolyDesc &desc,float density);
	const AABB2& getOOBB() const;
	const Sphere2& getBoundingSphere() const;
	BodyDesc & operator = (const BodyDesc & body);
protected:
	b2FixtureDef & addFixture(float density, b2Shape & shape);	/// create new fixture
	void updateBounds() const;	//< update bounds

	mutable bool bounds;			//< if bounds are updated
	mutable AABB2 boundBox;
	mutable Sphere2 boundSphere;
private:
	//
	void assign(const BodyDesc & desc);
};

// Contains Box2D solid body
class SolidObject
{
protected:
	Pose pose;    /// cached pose
	b2Body * solid;
	void updateSolidPose();  /// update solid body position
public:
	SolidObject();
	virtual ~SolidObject();

	bool isSolid()const;  /// if object has box2d body attached
	/// box2d handling
	void detachBody();
	void attachBody(b2Body * s);
	const b2Body* getBody() const;
	b2Body* getBody();
	void syncPose();	// sync cached pose with solid

	Pose::vec getVelocityLinear() const;
	Pose::vec3 getVelocityLinear3() const;
	/// Pose
	const Pose::pos & getPosition() const;
	Pose::vec getDirection() const;
	void setPosition(const Pose::pos &pos);
	void setDirection(const Pose::vec &dir);
	void setPose(const Pose &pose);
	Pose getPose() const;
	/// Geometry
	Sphere2 getBoundingSphere() const;
	AABB2	getOOBB()const;			// get object oriented bounding box
};

}
