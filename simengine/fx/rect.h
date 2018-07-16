#pragma once

#include <algorithm>
#include <cmath>

namespace Fx 
{

template<class T> struct RectBase
{
	T x1, y1, x2, y2;
	typedef RectBase<T> rect_type;

	RectBase(float _x1, float _y1, float _x2, float _y2)
	{
		x1=_x1;
		y1=_y1;
		x2=_x2;
		y2=_y2;
	}

	template<class Other> RectBase(const RectBase<Other> & rc)
		:x1(rc.x1), x2(rc.x2), y1(rc.y1), y2(rc.y2)
	{}

	RectBase()
	{
		clear();
	}

	void clear()
	{
		x1 = 0;
		x2 = -1;
		y1 = 0;
		y2 = -1;
	}

	bool isClean() const
	{
		return x1 > x2 && y1 > y2;
	}

	void set(float _x1, float _y1, float _x2, float _y2)
	{
		x1=_x1; x2=_x2; y1=_y1; y2=_y2;
	}

	void setRadius(float x, float y, float r)
	{
		x1 = x-r;
		x2 = x+r;
		y1 = y-r;
		y2 = y+r;
	}

	void encapsulate(float x, float y)
	{
		if(isClean())
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

	bool testPoint(float x, float y) const
	{
		if(x>=x1 && x<x2 && y>=y1 && y<y2) return true;

		return false;
	}

	bool intersect(const rect_type& rect) const
	{
		if(fabs(x1 + x2 - rect.x1 - rect.x2) < (x2 - x1 + rect.x2 - rect.x1))
			if(fabs(y1 + y2 - rect.y1 - rect.y2) < (y2 - y1 + rect.y2 - rect.y1))
				return true;

		return false;
	}

	T width() const
	{
		return x2 - x1;
	}
	T height() const
	{
		return y2 - y1;
	}
	T area() const
	{
		return width() * height();
	}

	void move(T x, T y)
	{
		x1 += x;
		y1 += y;
		x2 += x;
		y2 += y;
	}

	rect_type& operator = (const rect_type& rc)
	{
		x1 = rc.x1;
		y1 = rc.y1;
		x2 = rc.x2;
		y2 = rc.y2;
		return *this;
	}

	static rect_type intersect(const rect_type& a, const rect_type& b)
	{
		rect_type result;
		result.x1 = std::max(a.x1, b.x1);
		result.y1 = std::max(a.y1, b.y1);
		result.x2 = std::min(a.x2, b.x2);
		result.y2 = std::min(a.y2, b.y2);
		return result;
	}

	static rect_type merge(const rect_type& a, const rect_type& b)
	{
		rect_type result;
		result.x1 = std::min(a.x1, b.x1);
		result.y1 = std::min(a.y1, b.y1);
		result.x2 = std::max(a.x2, b.x2);
		result.y2 = std::max(a.y2, b.y2);
		return result;
	}
};

typedef RectBase<float> Rect;
typedef RectBase<int> IntRect;
}
