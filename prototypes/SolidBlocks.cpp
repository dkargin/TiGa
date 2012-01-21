// SolidBlocks.cpp : Defines the entry point for the console application.
//

#include "common.h"
#include "b2Tools.h"
#include "multiblock.h"
#include "bullet.h"

const float bulletLength = 1.0f;
typedef Pose2z Pose;

class Application : public BaseApp
{
public:
	b2World world;
	b2Body * groundBody;

	vec2i mouseStartPos;		// world mouse position
	vec2i mouseCurrentPos;		// current world mouse position
	bool targeting;				// if we are aiming bullet
	bool selecting;				// if we are picking cell

	std::list<Multiblock> bodies;
	std::list<Bullet> bullets;

	Multiblock & createBody();
	Bullet & createBullet();

	Application();
	virtual int demoStart();			// calls whed demo starts. GL system already started
	virtual int demoFinish();			// calls when demo finishes	
	virtual int demoRender();			// calls each frame
	virtual int	demoUpdate(float dt);
	virtual void onMouse(const vec2i & screenPos, int button, int state);
	virtual void onMouseMove( const vec2i & screenPos );

	void Draw(const Multiblock & cells)
	{		
		if(cells.body)
		{
			glEnable(GL_BLEND);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			//pushColor( 0.5, 0.5, 0.5);
			b2Fixture * selection = NULL;
			if( cells.selection && cells.selection->data.fixture )
				selection = cells.selection->data.fixture;

			glPushMatrix();			
			glTransform( GetPose( cells.body ) );			
			glPushMatrix();
			glScalef(cells.cellWidth, cells.cellWidth, 1);
			//glTranslatef( -0.5, -0.5, 0 );

			glBegin(GL_QUADS);
			if( cells.selection != NULL )
			{
				const float frameWidth = 0.5;
				glColor4f( 0.5, 0.5, 0.5, 0.2 );
				glVertex2f( 0 - frameWidth, 0 - frameWidth);
				glVertex2f( cells.MaxSize + frameWidth, 0 - frameWidth );
				glVertex2f( cells.MaxSize + frameWidth, cells.MaxSize + frameWidth );
				glVertex2f( 0 - frameWidth, cells.MaxSize + frameWidth );
			}
			for( size_t i = 0; i < cells.MaxCells; i++)
			{
				const Multiblock::Cell & cell = cells.cells[i];
				if( cell.state == Multiblock::Free ) continue;
				if( &cell == cells.selection )
					glColor3f( 1, 0, 0 );
				else if( cell.state == Multiblock::Moving )
					glColor3f( 0.5, 0.5, 0.5 );
				else
					glColor3f( 0.5, 0.5, 1.0 );

				float cellHalfWidth = 0.4;
				if( cell.x == cells.centerX && cell.y == cells.centerY )
					cellHalfWidth = 0.5;

				float x = cell.x;
				float y = cell.y;
				glVertex2f( x - cellHalfWidth, y - cellHalfWidth);
				glVertex2f( x + cellHalfWidth, y - cellHalfWidth);
				glVertex2f( x + cellHalfWidth, y + cellHalfWidth);
				glVertex2f( x - cellHalfWidth, y + cellHalfWidth);
			}
			
			glEnd();
			glPopMatrix();
			
			
			pushColor(0,1,0);
			//glBegin(GL_LINES);
			for( size_t i = 0; i < Multiblock::MaxLinks; ++i)
			{
				Multiblock::LinkRef ref = cells.getLinkRef(i);
				if( ref.link == NULL || ref.link->isFree() ) continue;
				int x = adjacentX( ref.dir, ref.x );
				int y = adjacentY( ref.dir, ref.y );
				drawArrow( vec2f( ref.x, ref.y) * cells.cellWidth, vec2f( x, y) * cells.cellWidth);
				//glVertex2f( x * cells.cellWidth, y * cells.cellWidth );
				//glVertex2f( ref.x * cells.cellWidth, ref.y * cells.cellWidth );
			}
			//glEnd();
			popColor();
			glPopMatrix();
			glColor4f( 0.5, 0.5, 0.5, 0.2 );
			::Draw(cells.body);
			//pushColor( 0.5, 1.0, 0.5);
			//cells.selection
		}
	};

	void Shoot( const vec2f & from, const vec2f & to );
	void Draw(const Bullet & bullet);

}application;

Multiblock::grid_type * Multiblock::fracture(int x, int y)
{
	Multiblock * result = NULL;
	if( application != NULL && body != NULL)
	{
		result = &application->createBody();
		Pose2 pose = GetPose(body);
		pose.position = pose.transformPos( vec2f(x,y) * cellWidth );
		result->init( pose, *body->GetWorld());
		result->body->SetLinearVelocity( body->GetLinearVelocityFromWorldPoint(conv(pose.position)));
		result->body->SetAngularVelocity( body->GetAngularVelocity() );
	}
	return result;
}

Application::Application()
	:world(b2Vec2(0,4)), groundBody(NULL), targeting(false), selecting(false), mouseCurrentPos(0,0)
{}

Multiblock & Application::createBody()
{
	bodies.push_back(Multiblock());
	Multiblock & result = bodies.back();
	result.application = this;
	return result;
}

Bullet & Application::createBullet()
{
	bullets.push_back(Bullet());
	return bullets.back();
}
const float speedModifier = 0.5f;
void Application::Shoot( const vec2f & from, const vec2f & to )
{
	Bullet & bullet = createBullet();
	bullet.pose.setPosition( from );
	bullet.pose.setDirection( to - from );
	bullet.speed = (to - from).length() / speedModifier;
	bullet.distance = 0;
	bullet.mass = 60.0f;
}

void Application::Draw(const Bullet & bullet)
{
	float bulletLength = 15;
	glPushMatrix();
	glTransform(bullet.pose);
	glTranslatef(bullet.distance, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(- std::min(bullet.distance, bulletLength), 0);
	glVertex2f(0,0);
	glEnd();
	glPopMatrix();
}

void Application::onMouse(const vec2i & screenPos, int button, int state)
{
	if( button == 0 && state == 0 )
	{
		mouseStartPos = screenToWorld(screenPos);
		targeting = true;
	}
	if( button == 0 && state == 1 )
	{
		// shoot
		targeting = false;
		Shoot(mouseStartPos, screenToWorld(screenPos));
	}

	if( button == 2 && state == 0 )
	{
		selecting = true;
	}

	if( button == 2 && state == 1 )
	{
		selecting = false;
	}

	if( button == 1 && state == 0 )
	{
		// 2. draw objects
		for( std::list<Multiblock>::iterator it = bodies.begin(); it != bodies.end(); it++ )
		{
			Multiblock & mb = *it;
			
			Multiblock::CellRef ref = mb.pick( mouseCurrentPos );
			if(ref.cell)
				mb.destroyCell( ref.x, ref.y );
		}
	}
}

void Application::onMouseMove( const vec2i & screenPos )
{
	mouseCurrentPos = screenToWorld(screenPos);
}

int Application::demoStart()
{
	// create world borders
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, 0.0f);
	groundBody = world.CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;

	const float sceneWidth = 200;
	const float border = 2;

	groundBox.SetAsBox(border, sceneWidth, b2Vec2( sceneWidth,0), 0);	groundBody->CreateFixture(&groundBox, 0.0);
	groundBox.SetAsBox(border, sceneWidth, b2Vec2(-sceneWidth,0), 0);	groundBody->CreateFixture(&groundBox, 0.0);
	groundBox.SetAsBox(sceneWidth, border, b2Vec2(0, sceneWidth), 0);	groundBody->CreateFixture(&groundBox, 0.0);
	groundBox.SetAsBox(sceneWidth, border, b2Vec2(0,-sceneWidth), 0);	groundBody->CreateFixture(&groundBox, 0.0);

	Multiblock & mb = createBody();
	mb.init(Pose2(0,0,M_PI/4), world);
	CellDesc data;
	const int size = 4;
	for( int x = 0; x < size; x++ )
		for( int y = 0; y < size; y++)
			mb.createCell(x,y,data); 

	//for( int y = 0; y < size; y++)
	//	mb.destroyCell(size/2, y);

	// Test link layout
	for( size_t i = 0; i < Multiblock::MaxCells; ++i)
	{
		Multiblock::LinkRef ref = mb.getLinkRef( &mb.links[i] );
		Multiblock::Link * link1 = mb.getLink( ref.x, ref.y, ref.dir );
		Multiblock::LinkRef ref2 = mb.getLinkRef( link1 );
		if( link1 != ref.link )
			int w = 0;
	}
	return 1;
}

int Application::demoUpdate(float dt)
{
	world.Step(dt, 8, 8);
	for( std::list<Bullet>::iterator it = bullets.begin(); it != bullets.end();  )
	{
		Bullet & bullet = *it;
		
		class RaycastCallback : public b2RayCastCallback 
		{
		public:
			bool hit;
			Multiblock * mb;
			Multiblock::Cell * cell;
			b2Vec2 point;
			b2Vec2 normal;
			RaycastCallback():hit(false), mb(NULL){}
			virtual float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& pt,
									const b2Vec2& norm, float32 fraction)
			{
				b2Body * body = fixture->GetBody();
				mb = (Multiblock*)body->GetUserData();
				if( mb )
				{
					cell = (Multiblock::Cell * ) fixture->GetUserData();
				}
				hit = true;
				point = pt;
				normal = norm;
				return fraction;
			}
		}callback;

		vec2f from = bullet.getPosition();
		vec2f to = from + bullet.getDirection() * bullet.speed * dt;
		world.RayCast(&callback, conv(from), conv(to) ); 
		if( callback.hit == false )
		{
			bullet.update( dt );
			++it;
		}
		else
		{
			if( callback.mb && callback.cell)
			{				
				float impulse = bullet.mass * bullet.speed ;
				callback.mb->hit( callback.cell, conv(callback.point), bullet.getDirection(), impulse );
				//callback.mb->destroyCell( callback.cell->x, callback.cell->y );
			}
			std::list<Bullet>::iterator it2 = it;
			it2++;
			bullets.erase(it);
			it = it2;
			
		}
	}
	return 1;
}

int Application::demoRender()
{
	pushColor(0,0,0);
	// 1. draw ground
	pushColor(0.5,0.5,0.5);	
	if(groundBody)
		::Draw(groundBody);
	popColor();
	// 2. draw objects
	for( std::list<Multiblock>::iterator it = bodies.begin(); it != bodies.end(); it++ )
	{
		Multiblock & mb = *it;
		if( selecting )
		{
			Multiblock::CellRef ref = mb.pick( mouseCurrentPos );
			if(ref.cell)
				mb.markSelected(ref.cell);
			else
				mb.markSelected( NULL );
		}
		Draw(*it);
	}
	pushColor( 1, 0, 0 );
	for( std::list<Bullet>::iterator it = bullets.begin(); it != bullets.end(); ++it )		
		Draw(*it);
	popColor();
	// 3. draw selection

	// 4. draw UI
	drawPoint(vec3(mouseCurrentPos[0], mouseCurrentPos[1], 0), 5, 0);

	if( targeting )
	{
		drawArrow( mouseStartPos, mouseCurrentPos );
	}
	popColor();
	return 1;
}

int Application::demoFinish()
{
	return 1;
}
