#include <box2d.h>

Pose2 GetPose(const b2Body * body);
void Draw(const b2Shape * shape);
void Draw(const b2Body * body, b2Fixture * selection = NULL);

inline const b2Vec2 & conv(const vec2f & v)
{
	return *(b2Vec2*)&v;
}

inline const vec2f & conv( const b2Vec2 &v)
{
	return *(vec2f*)&v;
}