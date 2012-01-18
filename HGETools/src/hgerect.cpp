/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeRect helper class implementation
*/

#include "stdafx.h"
#include "hgerect.h"

hgeRect::hgeRect(float _x1, float _y1, float _x2, float _y2) 
{
	x1=_x1; 
	y1=_y1; 
	x2=_x2; 
	y2=_y2;
}

hgeRect::hgeRect(const hgeRect & rc)
	:x1(rc.x1), x2(rc.x2), y1(rc.y1), y2(rc.y2)
{}

hgeRect::hgeRect() 
{
	Clear();
}

void    hgeRect::Clear() 
{
	x1 = 0;
	x2 = -1;
	y1 = 0;
	y2 = -1;	
}

bool    hgeRect::IsClean() const 
{
	return x1 > x2 && y1 > y2;
}

void	hgeRect::Set(float _x1, float _y1, float _x2, float _y2) 
{
	x1=_x1; x2=_x2; y1=_y1; y2=_y2; 
}

void	hgeRect::SetRadius(float x, float y, float r) 
{ 
	x1 = x-r; 
	x2 = x+r; 
	y1 = y-r; 
	y2 = y+r;
}
	

void hgeRect::Encapsulate(float x, float y)
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

bool hgeRect::TestPoint(float x, float y) const
{
	if(x>=x1 && x<x2 && y>=y1 && y<y2) return true;

	return false;
}

bool hgeRect::Intersect(const hgeRect *rect) const
{
	if(fabs(x1 + x2 - rect->x1 - rect->x2) < (x2 - x1 + rect->x2 - rect->x1))
		if(fabs(y1 + y2 - rect->y1 - rect->y2) < (y2 - y1 + rect->y2 - rect->y1))
			return true;

	return false;
}

hgeRect hgeRect::Intersect(const hgeRect & a, const hgeRect & b)
{
	hgeRect result;
	result.x1 = std::max(a.x1, b.x1);
	result.y1 = std::max(a.y1, b.y1);
	result.x2 = std::min(a.x2, b.x2);
	result.y2 = std::min(a.y2, b.y2);
	return result;
}

hgeRect hgeRect::Merge(const hgeRect & a, const hgeRect & b)
{
	hgeRect result;
	result.x1 = std::min(a.x1, b.x1);
	result.y1 = std::min(a.y1, b.y1);
	result.x2 = std::max(a.x2, b.x2);
	result.y2 = std::max(a.y2, b.y2);
	return result;
}