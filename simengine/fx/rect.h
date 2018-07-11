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

struct Rect
{
	typedef float Scalar;
	Scalar x1, y1, x2, y2;

	Rect(Scalar _x1, Scalar _y1, Scalar _x2, Scalar _y2);
	Rect();
	Rect(const Rect& rc);

	void clear();
	bool isClean() const;
	void set(Scalar _x1, Scalar _y1, Scalar _x2, Scalar _y2);
	void setRadius(Scalar x, Scalar y, Scalar r);
	void encapsulate(Scalar x, Scalar y);
	bool testPoint(Scalar x, Scalar y) const;
	bool intersect(const Rect& rect) const;

	Scalar width() const
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
	void move(Scalar x, Scalar y)
	{
		x1 += x;
		y1 += y;
		x2 += x;
		y2 += y;
	}
	Rect& operator = (const Rect& rc)
	{
		x1 = rc.x1;
		y1 = rc.y1;
		x2 = rc.x2;
		y2 = rc.y2;
		return *this;
	}
	static Rect intersect(const Rect& a, const Rect& b);
	static Rect merge(const Rect& a, const Rect& b);
};

}
