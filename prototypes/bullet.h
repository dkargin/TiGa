#ifndef _BULLET_H_
#define _BULLET_H_

typedef float DeltaTime;

class SceneObject
{
public:
	virtual ~SceneObject(){}
	virtual void update( DeltaTime time ) = 0;
	virtual void release() = 0;
};

class Bullet : public SceneObject
{
public:
	typedef Pose2 Pose;
	typedef Pose::pos pos;
	typedef Pose::dir dir;
	Pose2 pose;

	float speed;	// current speed
	float distance;	// distance travelled
	float mass;
	
	pos getPosition() const;
	dir getDirection() const;

	virtual void update( DeltaTime time );
	virtual void release() { delete this; }
};

#endif
