#include "debugDraw.h"

#include "GL/gl.h"
//#include "world.h"

#include "sim/commandAI.h"
#include "sim/inventory.h"
#include "sim/moverVehicle.h"
#include "sim/perception.h"
#include "sim/projectile.h"
#include "sim/unit.h"
#include "sim/weapon.h"

namespace sim
{

void drawLine(Fx::RenderContext* rc,const vec2f &from,const vec2f &to,Fx::FxRawColor color)
{
#ifdef FIX_THIS
	rc->Gfx_RenderLine(from[0],from[1],to[0],to[1]);
#endif
}

void drawArrow(Fx::RenderContext* rc,const vec2f &from,const vec2f &to,Fx::FxRawColor color,float width,float length)
{
#ifdef FIX_THIS
	auto dir=to-from;
	vec2f left(-dir[1],dir[0]);
	auto l=from-dir*0.2+left*0.1;
	auto r=from-dir*0.2-left*0.1;
	rc->Gfx_RenderLine(to[0],to[1],from[0],from[1]);
	rc->Gfx_RenderLine(to[0],to[1],r[0],r[1]);
	rc->Gfx_RenderLine(to[0],to[1],l[0],l[1]);
#endif
}

void drawPoint(Fx::RenderContext* rc,const vec2f &pos,Fx::FxRawColor color,float size,int style)
{
#ifdef FIX_THIS
	size/=2;
	rc->Gfx_RenderLine(pos[0]-size,pos[1]-size,pos[0]+size,pos[1]+size);
	rc->Gfx_RenderLine(pos[0]-size,pos[1]+size,pos[0]+size,pos[1]-size);
#endif
}

void Draw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
#ifdef FIX_THIS
	for (int32 i = 1; i <vertexCount; i++)
		rc->Gfx_RenderLine(vertices[i-1].x, vertices[i-1].y,  vertices[i].x, vertices[i].y, Fx::MakeARGB(200,color.r*128,color.g*128,color.b*128));
	rc->Gfx_RenderLine(vertices[vertexCount-1].x, vertices[vertexCount-1].y,  vertices[0].x, vertices[0].y, Fx::MakeARGB(200,color.r*128,color.g*128,color.b*128));
#endif
}

void Draw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
#ifdef FIX_THIS
	for (int32 i = 1; i <vertexCount; i++)
		rc->Gfx_RenderLine(vertices[i-1].x,  vertices[i-1].y, vertices[i].x, vertices[i].y, Fx::MakeARGB(200,color.r*128,color.g*128,color.b*128));
	rc->Gfx_RenderLine(vertices[vertexCount-1].x,  vertices[vertexCount-1].y, vertices[0].x, vertices[0].y, Fx::MakeARGB(200,color.r*128,color.g*128,color.b*128));
#endif
}

void Draw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
#ifdef FIX_THIS
   if(!rc) return;
   int NUMPOINTS = 24+(int)radius/20;;
   //Set up vertices for a circle
   //Used <= in the for statement to ensure last point meets first point (closed circle)
   float deltaAngle=2*M_PI/NUMPOINTS;
   for(int i=0;i<NUMPOINTS;i++)
   {
	   rc->Gfx_RenderLine(	center.x + radius*cosf(deltaAngle*i), 
							center.y + radius*sinf(deltaAngle*i),
							center.x + radius*cosf(deltaAngle*(i+1)), 
							center.y + radius*sinf(deltaAngle*(i+1)), Fx::MakeARGB(255,color.r*255,color.g*255,color.b*255));
   }
#endif
}

void Draw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
#ifdef FIX_THIS
	if(!rc) return;

	int NUMPOINTS = 24+(int)radius/20;

	float deltaAngle=2*M_PI/NUMPOINTS;
	for(int i=0;i<NUMPOINTS;i++)
	{
		rc->Gfx_RenderLine(center.x + radius*cosf(deltaAngle*i),
						center.y + radius*sinf(deltaAngle*i),
						center.x + radius*cosf(deltaAngle*(i+1)),
						center.y + radius*sinf(deltaAngle*(i+1)), Fx::MakeARGB(255,color.r*255,color.g*255,color.b*255));
	}
#endif
}

void Draw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
#ifdef FIX_THIS
   if(!rc)
  	 return;
   Fx::FxRawColor raw = Fx::MakeARGB(255, color.r*255, color.g*255, color.b*255);
   rc->Gfx_RenderLine(p1.x, p1.y, p2.x, p2.y, raw);
#endif
}

void Draw::DrawTransform(const b2Transform& xf)
{
#ifdef FIX_THIS
   if(!rc) return;

   b2Vec2 p1 = xf.p, p2;
   const float32 k_axisScale = 0.4f;
   
   p2 = p1 + b2Vec2( xf.q.GetXAxis() );
   rc->Gfx_RenderLine(p1.x,p1.y,p2.x,p2.y, Fx::MakeARGB(255,255,0,0));

   p2 = p1 + b2Vec2( xf.q.GetYAxis() );
   rc->Gfx_RenderLine(p1.x,p1.y,p2.x,p2.y, Fx::MakeARGB(255,0,255,0));
#endif
}

Draw::Draw()
{
	rc = nullptr;
	globalView.scale = 1.0;
	drawDynamics = true;
	drawDevices = false;
	drawTasks = true;
}

Draw::~Draw(void)
{}

void Draw::init(Fx::RenderContext* hge)
{	
	this->rc = hge;
}
//
//void Draw::begin()
//{
//	screenWidth = hge->System_GetState(HGE_SCREENWIDTH);
//	screenHeight = hge->System_GetState(HGE_SCREENHEIGHT);
//	float x = -globalView.position[0];
//	float y = -globalView.position[1];
//	float angle = globalView.orientation * 180/M_PI;
//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	glOrtho(-screenWidth/2,screenWidth/2,screenHeight/2,-screenHeight/2,-1,1);	
//	glScalef(globalScale,globalScale,1);
//	glRotatef(-angle,0,0,1);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	glTranslatef(x,y,0);
//
//	// draw view center
//	//drawPoint(hge, globalView.position,0x00ff00ff,20);
//
//	// TODO: Need to implement scrolling in screen coordinates
//	//	
//	//hge->Gfx_SetTransform(globalView.position[0]*globalScale-screenWidth/2,globalView.position[1]*globalScale-screenHeight/2,0,0,globalView.orientation,globalScale,globalScale);
//	//hge->Gfx_SetTransform(x,y,-x + screenWidth/2, -y + screenHeight/2,globalView.orientation,globalScale,globalScale);
//}
//
//void Draw::finish()
//{
//	glMatrixMode(GL_PROJECTION);
//	glPopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//}

// draws cone
void DrawCone(Fx::RenderContext* hge,const Pose &pose,float z,float fov,float distance,Fx::FxRawColor color)
{
	const float delta=0.1;

	Fx::VertexBatch batch(Fx::VertexBatch::PRIM_TRIPLES);
	batch.blend = BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE;
	
	for(float angle=-fov/2;angle < fov/2;angle+=delta)
	{
		float step=(angle+delta > fov/2)?fov/2-angle:delta;
		batch +=
		{
				Fx::Vertex::make3c(pose.position[0], pose.position[1], z, color),
				Fx::Vertex::make3c(
						pose.position[0]+distance*cosf(angle+pose.orientation),
						pose.position[1]+distance*sinf(angle+pose.orientation), z, color),
				Fx::Vertex::make3c(
						pose.position[0]+distance*cosf(angle+pose.orientation+step),
						pose.position[1]+distance*sinf(angle+pose.orientation+step), z, color)
		};
	}

	hge->renderBatch(batch);
}

void Draw::drawObject(GameObject* object)
{
	if(dynamic_cast<Unit*>(object))
	{
		draw(dynamic_cast<Unit*>(object));
	}
	else
	{
		queue->enqueue(object->getPose(), object->fx_root.shared_from_this());
	}
}

void Draw::drawSprite(const Pose &pose, const Fx::SpriteData *sprite)
{
	Fx::drawSprite(rc, *sprite, pose);
}

void Draw::draw(Unit* unit)
{
	if(!unit)
		return;
#ifdef TO_FIX
	Pose pose = unit->getPose();
	unit->effects->query(pose);
	for(int i = 0; i < unit->devices.size();i++)
	{
		Device *device = unit->devices[i];
		//int mount=device->mountIndex;
		if(device)
		{
			device->effects.query(pose);
			if( drawDevices )
				device->render(rc);
		}
	}

	if(unit->getController() && drawTasks)
		unit->getController()->render(rc);	
#endif
}

void drawSolidArrow(Fx::RenderContext* rc, const vec2f & from, const vec2f & to, float z, Fx::FxRawColor color, float width, float arrowWidth, float arrowLength)
{
	width /= 2;
	arrowWidth /= 2;

	Fx::VertexBatch body(Fx::VertexBatch::PRIM_TRIPLES);

	vec2f dir = to - from;
	float length = dir.length();
	if(length > 0.f)
		dir/=length;

	vec2f left(-dir[1], dir[0]);	

	float bodyLength = length - arrowLength;

	if( bodyLength > 0 )
	{
		vec2f bodyEnd = from + dir * bodyLength;

		vec2f quad[] =
		{
			from + left* width,
			from - left* width,
			bodyEnd - left* width,
			bodyEnd + left* width,
		};

		body +=
		{
			Fx::Vertex::make3c(quad[0][0], quad[0][1], z, color),
			Fx::Vertex::make3c(quad[1][0], quad[1][1], z, color),
			Fx::Vertex::make3c(quad[2][0], quad[2][1], z, color),

			Fx::Vertex::make3c(quad[0][0], quad[1][1], z, color),
			Fx::Vertex::make3c(quad[2][0], quad[2][1], z, color),
			Fx::Vertex::make3c(quad[3][0], quad[0][1], z, color),
		};
	}

	vec2f tip[] =
	{
		to,
		to - dir * arrowLength - left * arrowWidth,
		to - dir * arrowLength + left * arrowWidth
	};

	body +=
	{
		Fx::Vertex::make3c(tip[0][0], tip[0][1], z, color),
		Fx::Vertex::make3c(tip[1][0], tip[1][1], z, color),
		Fx::Vertex::make3c(tip[2][0], tip[2][1], z, color),
	};

	rc->renderBatch(body);
}

void DrawCircleArea(Fx::RenderContext* rc,const Pose& pose,float radius, Fx::FxRawColor color)
{

}

void Draw::draw(const VisionManager::VisionDesc& vision)
{
	if(vision.fov < 360)
	{
		DrawCone(rc,vision.pose,0,vision.fov*M_PI/180.f,vision.distance, Fx::MakeRGB(255, 255, 255));
	}
	else
	{
		DrawCone(rc,vision.pose,0,2*M_PI,vision.distance, Fx::MakeRGB(255, 255, 255));
		//DrawCircleArea(hge,vision.pose,vision.distance, Fx::MakeRGB(255, 255, 255));
	}
}

}
