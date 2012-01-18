/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeRect helper class
*/


#ifndef HGERECT_H
#define HGERECT_H


class hgeRect
{
public:
	typedef float Scalar;
	Scalar	x1, y1, x2, y2;

	hgeRect(Scalar _x1, Scalar _y1, Scalar _x2, Scalar _y2);
	hgeRect();
	hgeRect(const hgeRect & rc);

	void    Clear();
	bool    IsClean() const;
	void	Set(Scalar _x1, Scalar _y1, Scalar _x2, Scalar _y2);
	void	SetRadius(Scalar x, Scalar y, Scalar r);
	void	Encapsulate(Scalar x, Scalar y);
	bool	TestPoint(Scalar x, Scalar y) const;
	bool	Intersect(const hgeRect *rect) const;
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
	hgeRect & operator = (const hgeRect & rc)
	{
		x1 = rc.x1;
		y1 = rc.y1;
		x2 = rc.x2;
		y2 = rc.y2;
		return *this;
	}
	static hgeRect Intersect(const hgeRect & a, const hgeRect & b);
	static hgeRect Merge( const hgeRect & a, const hgeRect & b);
};
#endif
