/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** SpriteData helper class implementation
*/

#include <cmath>
#include "spritedata.h"

namespace Fx
{

SpriteData::SpriteData(FxTextureId texture, int texWidth_, int texHeight_, float texx, float texy, float w, float h)
{
	float texx1, texy1, texx2, texy2;
	
	src.x1 = texx;
	src.y1 = texy;
	src.x2 = texx + w;
	src.y2 = texy + h;

	if(texture)
	{
		this->texWidth = texWidth_;
		this->texHeight = texHeight_;
	}
	else
	{
		texWidth = 1.0f;
		texHeight = 1.0f;
	}

	src.set(texx, texy, texx+w, texy+h);

	hotX=0;
	hotY=0;
	tex=texture;

	texx1=texx/texWidth;
	texy1=texy/texHeight;
	texx2=(texx+w)/texWidth;
	texy2=(texy+h)/texHeight;

	blend = BLEND_DEFAULT;
	z = 0;
	color = 0xffffffff;
	blend = 0;
}

#ifdef DEPRECATED_CODE
void SpriteData::Render(float x, float y)
{
	float tempx1, tempy1, tempx2, tempy2;

	tempx1 = x-hotX;
	tempy1 = y-hotY;
	tempx2 = x+width-hotX;
	tempy2 = y+height-hotY;

	quad.v[0].x = tempx1; quad.v[0].y = tempy1;
	quad.v[1].x = tempx2; quad.v[1].y = tempy1;
	quad.v[2].x = tempx2; quad.v[2].y = tempy2;
	quad.v[3].x = tempx1; quad.v[3].y = tempy2;

	hge->Gfx_RenderQuad(&quad);
}


void SpriteData::RenderEx(float x, float y, float rot, float hscale, float vscale)
{
	float tx1, ty1, tx2, ty2;
	float sint, cost;

	if(vscale==0) vscale=hscale;

	tx1 = -hotX*hscale;
	ty1 = -hotY*vscale;
	tx2 = (width-hotX)*hscale;
	ty2 = (height-hotY)*vscale;

	if (rot != 0.0f)
	{
		cost = cosf(rot);
		sint = sinf(rot);
			
		quad.v[0].x  = tx1*cost - ty1*sint + x;
		quad.v[0].y  = tx1*sint + ty1*cost + y;	

		quad.v[1].x  = tx2*cost - ty1*sint + x;
		quad.v[1].y  = tx2*sint + ty1*cost + y;	

		quad.v[2].x  = tx2*cost - ty2*sint + x;
		quad.v[2].y  = tx2*sint + ty2*cost + y;	

		quad.v[3].x  = tx1*cost - ty2*sint + x;
		quad.v[3].y  = tx1*sint + ty2*cost + y;	
	}
	else
	{
		quad.v[0].x = tx1 + x; quad.v[0].y = ty1 + y;
		quad.v[1].x = tx2 + x; quad.v[1].y = ty1 + y;
		quad.v[2].x = tx2 + x; quad.v[2].y = ty2 + y;
		quad.v[3].x = tx1 + x; quad.v[3].y = ty2 + y;
	}

	hge->Gfx_RenderQuad(&quad);
}


void SpriteData::RenderStretch(float x1, float y1, float x2, float y2)
{
	quad.v[0].x = x1; quad.v[0].y = y1;
	quad.v[1].x = x2; quad.v[1].y = y1;
	quad.v[2].x = x2; quad.v[2].y = y2;
	quad.v[3].x = x1; quad.v[3].y = y2;

	hge->Gfx_RenderQuad(&quad);
}


void SpriteData::Render4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
	quad.v[0].x = x0; quad.v[0].y = y0;
	quad.v[1].x = x1; quad.v[1].y = y1;
	quad.v[2].x = x2; quad.v[2].y = y2;
	quad.v[3].x = x3; quad.v[3].y = y3;

	hge->Gfx_RenderQuad(&quad);
}
#endif

Rect* SpriteData::getBoundingBoxEx(float x, float y, float rot, float hscale, float vscale, Rect *rect) const
{
	float tx1, ty1, tx2, ty2;
	float sint, cost;

	rect->clear();
	
	tx1 = -hotX*hscale;
	ty1 = -hotY*vscale;
	tx2 = (src.width()-hotX)*hscale;
	ty2 = (src.height()-hotY)*vscale;

	if (rot != 0.0f)
	{
		cost = cosf(rot);
		sint = sinf(rot);
			
		rect->encapsulate(tx1*cost - ty1*sint + x, tx1*sint + ty1*cost + y);	
		rect->encapsulate(tx2*cost - ty1*sint + x, tx2*sint + ty1*cost + y);	
		rect->encapsulate(tx2*cost - ty2*sint + x, tx2*sint + ty2*cost + y);	
		rect->encapsulate(tx1*cost - ty2*sint + x, tx1*sint + ty2*cost + y);	
	}
	else
	{
		rect->encapsulate(tx1 + x, ty1 + y);
		rect->encapsulate(tx2 + x, ty1 + y);
		rect->encapsulate(tx2 + x, ty2 + y);
		rect->encapsulate(tx1 + x, ty2 + y);
	}

	return rect;
}

void SpriteData::setTexture(FxTextureId tex_, int width, int height)
{
	float tx1,ty1,tx2,ty2;

	tex = tex_;

	if(tex)
	{
		texWidth = width;
		texHeight = width;
	}
	else
	{
		texWidth = 1.0f;
		texHeight = 1.0f;
	}

	/*
	if(tw!=texWidth || th!=texHeight)
	{
		// Doing recalculation for new size?
		tx1=quad.v[0].tx*texWidth;
		ty1=quad.v[0].ty*texHeight;
		tx2=quad.v[2].tx*texWidth;
		ty2=quad.v[2].ty*texHeight;

		tx1/=tw; ty1/=th;
		tx2/=tw; ty2/=th;

		quad.v[0].tx=tx1; quad.v[0].ty=ty1; 
		quad.v[1].tx=tx2; quad.v[1].ty=ty1; 
		quad.v[2].tx=tx2; quad.v[2].ty=ty2; 
		quad.v[3].tx=tx1; quad.v[3].ty=ty2;
	}
	*/
}


void SpriteData::setTextureRect(float x, float y, float w, float h, bool adjSize)
{
	float tx1, ty1, tx2, ty2;
	bool bX,bY,bHS;

	src.x1 = x;
	src.y1 = y;
	
	if(adjSize)
	{
		//width = w;
		//height = h;
	}

	tx1=src.x1/texWidth;
	ty1=src.y1/texHeight;
	tx2=(src.x1+w)/texWidth;
	ty2=(src.y1+h)/texHeight;

	/*
	quad.v[0].tx=tx1; quad.v[0].ty=ty1; 
	quad.v[1].tx=tx2; quad.v[1].ty=ty1; 
	quad.v[2].tx=tx2; quad.v[2].ty=ty2; 
	quad.v[3].tx=tx1; quad.v[3].ty=ty2; 

	bXFlip=false; bYFlip=false;
	*/
}


void SpriteData::setColor(FxRawColor col)
{
	color = col;
}

void SpriteData::setZ(float z)
{
	this->z = z;
}

void SpriteData::setBlendMode(int blend)
{
	this->blend = blend;
}

void SpriteData::setHotSpot(float x, float y)
{
	hotX=x;
	hotY=y;
}

FxTextureId SpriteData::getTexture() const
{
	return tex;
}

Rect SpriteData::getTextureRect() const
{
	return src;
}

FxRawColor SpriteData::getColor() const
{
	return color;
}

float SpriteData::getZ() const
{
	return z;
}

int SpriteData::getBlendMode() const
{
	return blend;
}

void SpriteData::getHotSpot(float *x, float *y) const
{
	*x=hotX;
	*y=hotY;
}

float SpriteData::getWidth() const
{
	return src.width();
}

float SpriteData::getHeight() const
{
	return src.height();
}

Rect* SpriteData::getBoundingBox(float x, float y, Rect *rect) const
{
	rect->set(x-hotX, y-hotY, x-hotX+src.width(), y-hotY+src.height());
	return rect;
}

}

