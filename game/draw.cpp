#include "stdafx.h"
#include "gl/gl.h"
#include "unit.h"
//#include "gameObject.h"
#include "perception.h"
#include "moverVehicle.h"
#include "projectile.h"
#include "inventory.h"
#include "weapon.h"
#include "world.h" 
#include "commandAI.h"

#include "draw.h"
extern float screenWidth,screenHeight;

void drawLine(HGE * hge,const vec2f &from,const vec2f &to,DWORD color)
{
	hge->Gfx_RenderLine(from[0],from[1],to[0],to[1]);
}

void drawArrow(HGE * hge,const vec2f &from,const vec2f &to,DWORD color,float width,float length)
{
	auto dir=to-from;
	vec2f left(-dir[1],dir[0]);
	auto l=from-dir*0.2+left*0.1;
	auto r=from-dir*0.2-left*0.1;
	hge->Gfx_RenderLine(to[0],to[1],from[0],from[1]);
	hge->Gfx_RenderLine(to[0],to[1],r[0],r[1]);
	hge->Gfx_RenderLine(to[0],to[1],l[0],l[1]);
}

void drawPoint(HGE * hge,const vec2f &pos,DWORD color,float size,int style)
{
	size/=2;
	hge->Gfx_RenderLine(pos[0]-size,pos[1]-size,pos[0]+size,pos[1]+size);
	hge->Gfx_RenderLine(pos[0]-size,pos[1]+size,pos[0]+size,pos[1]-size);
}

void Draw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	for (int32 i = 1; i <vertexCount; i++)
		hge->Gfx_RenderLine(vertices[i-1].x, vertices[i-1].y,  vertices[i].x, vertices[i].y, ARGB(200,color.r*128,color.g*128,color.b*128));
	hge->Gfx_RenderLine(vertices[vertexCount-1].x, vertices[vertexCount-1].y,  vertices[0].x, vertices[0].y, ARGB(200,color.r*128,color.g*128,color.b*128));
}

void Draw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{	
   for (int32 i = 1; i <vertexCount; i++)
      hge->Gfx_RenderLine(vertices[i-1].x,  vertices[i-1].y, vertices[i].x, vertices[i].y, ARGB(200,color.r*128,color.g*128,color.b*128));
	hge->Gfx_RenderLine(vertices[vertexCount-1].x,  vertices[vertexCount-1].y, vertices[0].x, vertices[0].y, ARGB(200,color.r*128,color.g*128,color.b*128));
}

void Draw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
   if(!hge) return;
   int NUMPOINTS = 24+(int)radius/20;;
   //Set up vertices for a circle
   //Used <= in the for statement to ensure last point meets first point (closed circle)
   float deltaAngle=2*M_PI/NUMPOINTS;
   for(int i=0;i<NUMPOINTS;i++)
   {
	   hge->Gfx_RenderLine(	center.x + radius*cosf(deltaAngle*i), 
							center.y + radius*sinf(deltaAngle*i),
							center.x + radius*cosf(deltaAngle*(i+1)), 
							center.y + radius*sinf(deltaAngle*(i+1)), ARGB(255,color.r*255,color.g*255,color.b*255));
   }
}

void Draw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
   if(!hge) return;  

   int NUMPOINTS = 24+(int)radius/20;
 
   float deltaAngle=2*M_PI/NUMPOINTS;
   for(int i=0;i<NUMPOINTS;i++)
   {
	   hge->Gfx_RenderLine(	center.x + radius*cosf(deltaAngle*i), 
							center.y + radius*sinf(deltaAngle*i),
							center.x + radius*cosf(deltaAngle*(i+1)), 
							center.y + radius*sinf(deltaAngle*(i+1)), ARGB(255,color.r*255,color.g*255,color.b*255));
   }
}

void Draw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
   if(!hge) return;
   hge->Gfx_RenderLine(p1.x,  p1.y,  p2.x,  p2.y, ARGB(255,color.r*255,color.g*255,color.b*255));
}

void Draw::DrawTransform(const b2Transform& xf)
{
   if(!hge) return;

   b2Vec2 p1 = xf.p, p2;
   const float32 k_axisScale = 0.4f;
   
   p2 = p1 + b2Vec2( xf.q.GetXAxis() );
   hge->Gfx_RenderLine(p1.x,p1.y,p2.x,p2.y, ARGB(255,255,0,0));

   p2 = p1 + b2Vec2( xf.q.GetYAxis() );
   hge->Gfx_RenderLine(p1.x,p1.y,p2.x,p2.y, ARGB(255,0,255,0));
}

////////////////////////////////////////////////////////////////////
// Draw. Рисует всю сцену.
////////////////////////////////////////////////////////////////////
Draw::Draw()
:local(Mt3x3::identity()),hge(NULL)
{
	globalView.scale = 1.0;
	drawDynamics = true;
	drawDevices = false;
	drawTasks = true;
}

Draw::~Draw(void)
{}

void Draw::init(HGE *hge)
{	
	this->hge = hge;
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
void DrawCone(HGE *hge,const Pose &pose,float z,float fov,float distance,DWORD color)
{
	const float delta=0.1;
	hgeTriple triple;

	triple.tex=0;
	for(int i=0;i<3;i++)
	{
		triple.v[i].z=z;	
		triple.v[i].col=color;	
	}
	triple.v[0].x=pose.position[0];
	triple.v[0].y=pose.position[1];
	triple.blend=BLEND_COLORADD | BLEND_ALPHABLEND | BLEND_NOZWRITE;
	
	for(float angle=-fov/2;angle < fov/2;angle+=delta)
	{
		float step=(angle+delta > fov/2)?fov/2-angle:delta;
		triple.v[1].x=pose.position[0]+distance*cosf(angle+pose.orientation);
		triple.v[1].y=pose.position[1]+distance*sinf(angle+pose.orientation);
		triple.v[2].x=pose.position[0]+distance*cosf(angle+pose.orientation+step);
		triple.v[2].y=pose.position[1]+distance*sinf(angle+pose.orientation+step);

		hge->Gfx_RenderTriple(&triple);
	}
}

void Draw::drawObject(GameObject *object)
{
	if(dynamic_cast<Unit*>(object))
		draw(dynamic_cast<Unit*>(object));
	else if( object->effects )
	{
		FxEffect::Pointer ptr = object->effects;
		object->effects->queryAll(ptr->getManager(), object->getPose());
	}
}

void Draw::drawSprite(const Pose &pose,hgeSprite *sprite)
{
	sprite->RenderEx(pose.position[0],pose.position[1],pose.orientation);
}

void Draw::draw(Unit *unit)
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
				device->render(hge);
		}
	}

	if(unit->getController() && drawTasks)
		unit->getController()->render(hge);	
#endif
}

void drawSolidArrow(HGE * hge, const vec2f & from, const vec2f & to, float z, DWORD color, float width, float arrowWidth, float arrowLength)
{		
	auto assign = [color,z](hgeVertex & target, const vec2f & from)
	{
		target.x = from[0];
		target.y = from[1];
		target.z = z;
		target.tx = 0;
		target.ty = 0;
		target.col = color;
	};

	width /= 2;
	arrowWidth /= 2;

	vec2f dir = to - from;
	float length = dir.length();
	if(length > 0.f)
		dir/=length;
	vec2f left(-dir[1], dir[0]);	

	float bodyLength = length - arrowLength;
	if( bodyLength > 0 )
	{
		hgeQuad body;
		body.tex = 0;
		body.blend = 0;
		vec2f bodyEnd = from + dir * bodyLength;
		assign(body.v[0],from + left* width);
		assign(body.v[1],from - left* width);
		assign(body.v[2],bodyEnd - left* width);
		assign(body.v[3],bodyEnd + left* width);
		hge->Gfx_RenderQuad(&body);
	}

	hgeTriple tip;
	tip.tex = 0;
	tip.blend = 0;
	assign(tip.v[0],to);
	assign(tip.v[1],to - dir * arrowLength - left * arrowWidth);
	assign(tip.v[2],to - dir * arrowLength + left * arrowWidth);
	hge->Gfx_RenderTriple(&tip);
}

void DrawCircleArea(HGE* hge,const Pose& pose,float radius,DWORD color)
{

}

void Draw::draw(const VisionManager::Vision &vision)
{
	if(vision.fov<360)
		DrawCone(hge,vision.pose,0,vision.fov*M_PI/180.f,vision.distance,hgeColorRGB(1,1,1,1).GetHWColor()); 
	else
		DrawCone(hge,vision.pose,0,2*M_PI,vision.distance,hgeColorRGB(1,1,1,1).GetHWColor()); 
		//DrawCircleArea(hge,vision.pose,vision.distance,hgeColorRGB(1,1,1,1).GetHWColor());
}