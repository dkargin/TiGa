/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Rect helper class implementation
*/

#include <algorithm>
#include "rect.h"

namespace Fx
{

Rect::Rect(float _x1, float _y1, float _x2, float _y2) 
{
	x1=_x1; 
	y1=_y1; 
	x2=_x2; 
	y2=_y2;
}

Rect::Rect(const Rect & rc)
	:x1(rc.x1), x2(rc.x2), y1(rc.y1), y2(rc.y2)
{}

Rect::Rect() 
{
	Clear();
}

void Rect::Clear()
{
	x1 = 0;
	x2 = -1;
	y1 = 0;
	y2 = -1;	
}

bool Rect::IsClean() const
{
	return x1 > x2 && y1 > y2;
}

void Rect::Set(float _x1, float _y1, float _x2, float _y2)
{
	x1=_x1; x2=_x2; y1=_y1; y2=_y2; 
}

void Rect::SetRadius(float x, float y, float r)
{ 
	x1 = x-r; 
	x2 = x+r; 
	y1 = y-r; 
	y2 = y+r;
}
	

void Rect::Encapsulate(float x, float y)
{
	if(IsClean())
	{
		x1 = x2 = x;
		y1 = y2 = y;
	}
	else
	{
		if(x<x1) x1=x;
		if(x>x2) x2=x;
		if(y<y1) y1=y;
		if(y>y2) y2=y;
	}
}

bool Rect::TestPoint(float x, float y) const
{
	if(x>=x1 && x<x2 && y>=y1 && y<y2) return true;

	return false;
}

bool Rect::Intersect(const Rect *rect) const
{
	if(fabs(x1 + x2 - rect->x1 - rect->x2) < (x2 - x1 + rect->x2 - rect->x1))
		if(fabs(y1 + y2 - rect->y1 - rect->y2) < (y2 - y1 + rect->y2 - rect->y1))
			return true;

	return false;
}

Rect Rect::Intersect(const Rect & a, const Rect & b)
{
	Rect result;
	result.x1 = std::max(a.x1, b.x1);
	result.y1 = std::max(a.y1, b.y1);
	result.x2 = std::min(a.x2, b.x2);
	result.y2 = std::min(a.y2, b.y2);
	return result;
}

Rect Rect::Merge(const Rect & a, const Rect & b)
{
	Rect result;
	result.x1 = std::min(a.x1, b.x1);
	result.y1 = std::min(a.y1, b.y1);
	result.x2 = std::max(a.x2, b.x2);
	result.y2 = std::max(a.y2, b.y2);
	return result;
}
}
