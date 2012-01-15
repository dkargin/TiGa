#include "common.h"
#include "b2Tools.h"

Pose2 GetPose(const b2Body * body)
{
	Pose2 pose;
	pose.orientation = body->GetAngle();
	b2Vec2 pos = body->GetPosition();
	pose.position[0] = pos.x;
	pose.position[1] = pos.y;
	return pose;
}

void Draw(const b2Shape * shape)
{
	if( shape->GetType() == b2Shape::e_polygon )
	{
		const b2PolygonShape * pShape = (const b2PolygonShape *) shape;
		size_t size = pShape->GetVertexCount();
		glBegin(GL_LINE_LOOP);
		for( size_t i = 0; i < size; ++i )
		{
			b2Vec2 current = pShape->GetVertex(i);
			glVertex2f(current.x, current.y);
		}
		glEnd();
	}
}

void Draw(const b2Body * body, b2Fixture * selection)
{
	glPushMatrix();
	glTransform( GetPose( body ) );	

	const b2Fixture * fixture = body->GetFixtureList();
	while(fixture)
	{
		const b2Shape * shape = fixture->GetShape();
		
		glLineWidth( fixture == selection ? 2 : 1 );
		if(shape)
			Draw(shape);

		fixture = fixture->GetNext();
	}
	glLineWidth(1);
	glPopMatrix();
}