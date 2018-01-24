/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeRect helper class
*/


#pragma once

namespace Fx 
{

class Rect
{
public:
	typedef float Scalar;
	Scalar	x1, y1, x2, y2;

	Rect(Scalar _x1, Scalar _y1, Scalar _x2, Scalar _y2);
	Rect();
	Rect(const Rect & rc);

	void    Clear();
	bool    IsClean() const;
	void	Set(Scalar _x1, Scalar _y1, Scalar _x2, Scalar _y2);
	void	SetRadius(Scalar x, Scalar y, Scalar r);
	void	Encapsulate(Scalar x, Scalar y);
	bool	TestPoint(Scalar x, Scalar y) const;
	bool	Intersect(const Rect *rect) const;
	Scalar	width() const
	{
		return x2 - x1;
	}
	Scalar height() const
	{
		return y2 - y1;
	}
	Scalar area() const
	{
		return width() * height();
	}
	void	move(Scalar x, Scalar y)
	{
		x1 += x;
		y1 += y;
		x2 += x;
		y2 += y;
	}
	Rect & operator = (const Rect & rc)
	{
		x1 = rc.x1;
		y1 = rc.y1;
		x2 = rc.x2;
		y2 = rc.y2;
		return *this;
	}
	static Rect Intersect(const Rect & a, const Rect & b);
	static Rect Merge( const Rect & a, const Rect & b);
};

}
