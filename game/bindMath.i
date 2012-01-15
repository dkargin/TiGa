%{
#include "gameObject.h"
%}

class vec2f
{
public:
	float c[2];
	vec2f();
	vec2f(float a,float b);
	~vec2f();
};
// Add a new attribute to vec2f
%extend vec2f
{
    float x;
	float y;
	const float length;
}
%{
// x coordinate
float vec2f_x_get(vec2f *v) 
{
	return v->c[0];
}
void vec2f_x_set(vec2f * v, float val)
{
	v->c[0]=val;
}
// y coordinate
float vec2f_y_get(vec2f *v) 
{
	return v->c[1];
}
void vec2f_y_set(vec2f * v, float val)
{
	v->c[1]=val;
}
// vector length, read only
const float vec2f_length_get(vec2f *v)
{
	return v->length();
}
%}

class Pose
{
public:
	typedef vec2f pos;		/// describes "position" type
	typedef vec3f pos3;		/// describes "position" type
	typedef vec2f vec;		/// describes "vector" type
	typedef vec dir;		/// describes "direction" type. Actually is syninonim to "vector"
	typedef float rot;		/// describes "rotation" type.
	typedef Mt3x3 mat;		/// describes "matrix" type

	pos3 position;			/// object global position
	rot orientation;		/// object global orientation. For 2d pose it's scalar angle(rad). For 3d - quaternion
	
	Pose():position(0.0f),orientation(0.0){}
	Pose(float x,float y,float rot);
	Pose(float x,float y,float rot,float z);
	Pose(const Pose &pose);
	Pose(const Pose::pos &p,const Pose::rot &r);
	~Pose();
	pos getPosition() const;
	pos coords(float x,float y)const;
	void setPosition(const pos &pos);
	/// returns object direction. Equal to X axis
	dir getDirection() const;
	/// set pose direction. Equal to setting X axis
	void setDirection(const dir &d);
	/// convert to matrix form
	mat getMat() const;
	/// get X axis orth in world frame
	inline vec axisX() const;
	/// get Y axis orth in world frame
	inline vec axisY() const;
	/// convert from local direction to world direction(vector)
	inline vec transformVec(const vec &v) const;
	/// convert from local position to world position
	inline pos transformPos(const pos &p) const;
	/// convert from world position to local position (untranslate+unrotate)
	inline pos projectPos(const pos &p) const;
	/// convert from world direction to local direction (unrotate)
	inline vec projectVec(const vec &v) const;
};
// Add a new attribute to vec2f
%extend Pose
{
    float x;
	float y;
}
%{
// x coordinate
float Pose_x_get(Pose *v) 
{
	return v->position[0];
}
void Pose_x_set(Pose * v, float val)
{
	v->position[0]=val;
}
// y coordinate
float Pose_y_get(Pose *v) 
{
	return v->position[1];
}
void Pose_y_set(Pose * v, float val)
{
	v->position[1]=val;
}
%}
inline Pose operator * (const Pose &a,const Pose &b);

#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.434294481903251827651
#define M_LN2      0.693147180559945309417
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M_1_PI     0.318309886183790671538
#define M_2_PI     0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.707106781186547524401

float DEG2RAD(float a);
float RAD2DEG(float a);