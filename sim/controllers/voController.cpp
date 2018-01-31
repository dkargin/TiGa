#include "voController.h"

#include "../device.h"
#include "../mover.h"
#include "../moverVehicle.h"
#include "../unit.h"

namespace sim
{

const vec2f &to2d(const vec2f &v)
{
	return v;
}

void rotate90(vec2f & vec)
{
	/*
	x1 = -y0
	y1 = x1
	*/
	float * c = vec;
	float tmp = c[0]; c[0] = -c[1]; c[1] = tmp;
}

void rotate90n(vec2f & vec)
{
	/*
	x1 = -y0
	y1 = x1
	*/
	float * c = vec;
	float tmp = c[0]; c[0] = c[1]; c[1] = -tmp;
}

typedef Unit Object;

FlatCone::FlatCone(const Pose::vec& posA,float sizeA,const Pose::vec& posB,float sizeB,const Pose::vec& vel)
{
	apex = posA + vel;				// ������� ������
	float size = sizeA + sizeB;		// ��������� ������ - �� �� ������ �������� ����������. Ÿ ����� ��������� � ������� B
	Pose::vec dir=posB-posA;		
	float angle=0;
	if(dir.length() < size)	
		 angle=M_PI/2;			// ���� ������ ������� ������. ����� ����������� � �������������
	else
	{
		//assert(dir.length_squared()-size*size);						// ��� �� ������ ���� �� ��� ������ ������� �������. ���� ����� ��� ����� ������
		float touchDistance=sqrt(dir.length_squared()-size*size);	// ���������� �� ������ A �� ����� �������. �� ������� ��������. 
		angle=acos(touchDistance/dir.length());				// ��� ���� ��� ��� ��� ������. ��� �������� ���		
	}

	dirLeft = vec2f::normalize_s(dir).rotate(angle);						// �������� ����������� ����� �����������
	dirRight = vec2f::normalize_s(dir).rotate(-angle);					// �������� ����������� ������ �����������
	flags = 0;
	if(apex.isZero())
		flags |= zeroApex;
	if(( dirLeft[1] * apex[0] - dirLeft[0] * apex[1]) < 0.f && (dirRight[1] * apex[0] - dirRight[0] * apex[1] > 0.f))
		flags |= zeroInside;

	distanceLeft = dirLeft[1] * apex[0] - dirLeft[0] * apex[1];	// ���������� �� ������ 
	distanceRight = dirRight[1] * apex[0] - dirRight[0] * apex[1];	// ���������� �� ������ 
}

// ������������ ������� ��������� , ���������� VelocityObstacle
void addSegment(math3::Segments & segments, const Sphere2& circle, const VelocityObstacle &vo)
{
	typedef Sphere2 Circle;
	//Circle circle((const float*)mover->getGlobalPose().position,maxVelocity);	
	Edge2 left(to2d(vo.apex),to2d(vo.apex+vo.dirLeft));
	Edge2 right(to2d(vo.apex),to2d(vo.apex+vo.dirRight));
	vec2f res_v[4];
	float res_t[4];
	int resLeft = intersection(left,circle,res_v,res_t);		
	int resRight = intersection(right,circle,res_v+resLeft,res_t+resLeft);
	float angles[4];
	for(int i=0;i<resLeft + resRight; i++)
	{
		vec2f dir = res_v[i]-circle.center;
		angles[i] = atan2(dir[1],dir[0]);
	}	
	if(vecDistance(circle.center,to2d(vo.apex)) < circle.radius)	// VO slices only 1 segment
	{
		math3::Segment result;
		vec2f dirLeft=res_v[1]-circle.center;		
		vec2f dirRight=res_v[resLeft + 1]-circle.center;
		result.max=atan2(dirLeft[1],dirLeft[0]);
		result.min=atan2(dirRight[1],dirRight[0]);		
		segments |= result;
	}
	else	// VO slices 2 segments on the circle
	{
		
		if(res_t[0]<0.f && res_t[resLeft]<0.f)
			return;
			
		if(resLeft + resRight == 4)
		{
			segments|=math3::Segment(angles[0],angles[2]);
			segments|=math3::Segment(angles[3],angles[1]);
		}
		else if (resLeft == 1 && resRight == 2)
		{
			segments|=math3::Segment(angles[0],angles[1]);
		}
		else if (resLeft == 2 && resRight == 1)
		{
			segments|=math3::Segment(angles[1],angles[0]);
		}
		else if (resLeft == 1 && resRight == 1)
		{
			segments|=math3::Segment::makeFull();
		}
	}
	//return result;	
	
}

// intersection between ray going from (0,0) and edge
inline int VelocityObstacle::intersectionLeft(const vec2f &ray, float res_t[2]) const
{
	float normProj = (dirLeft[1] * ray[0] - dirLeft[0] * ray[1]);			// ����� �������� ���� �� �������
	// if ray and edge are parallel
	if( normProj == 0.f )
		return 0;	
	res_t[0] = distanceLeft/normProj;
	res_t[1] = dirLeft & (ray*res_t[0] - apex);
	return 1;
}

// intersection between ray going from (0,0) and edge
inline int VelocityObstacle::intersectionRight(const vec2f &ray, float res_t[2]) const
{
	float normProj = (dirRight[1] * ray[0] - dirRight[0] * ray[1]);			// ����� �������� ���� �� �������
	// if ray and edge are parallel
	if( normProj == 0.f )
		return 0;	
	res_t[0] = distanceRight/normProj;
	res_t[1] = dirRight & (ray*res_t[0] - apex);
	return 1;
}

Range VelocityObstacle::rayCast(const vec2f & ray)const
{	
	if(flags & Flags::zeroApex) 
	{
		typedef Edge2 Line;
		Line left(apex,apex + dirLeft);
		Line right(apex,apex + dirRight);
		if(classify(left,ray) == math3::geometry::cRIGHT &&
				classify(right,ray) == math3::geometry::cLEFT)
			return Range(0,Range::Basic,0,Range::Inf);
		return Range::makeZero();
	}
	else
	{		
		float lt[2],rt[2];
		// get intersections
		int resLeft = intersectionLeft(ray, lt);
		int resRight = intersectionRight(ray, rt);
		// check if the intersections are valid
		bool hitLeft = resLeft && lt[0]>=0.f && lt[1]>=0.f;
		bool hitRight = resRight && rt[0]>=0.f && rt[1]>=0.f;
		// if ray is in VO		
		// ��� ����� ���� ����� ����������
		// ���������, ��������� �� ����� 0,0 ������ ������
		if(flags & Flags::zeroInside)
		{
			if(hitLeft && hitRight)
			{
				if(lt[1]>0.f && rt[1]>0.f)
					return Range(lt[0],rt[0]);
				if(lt[1]==0.f && rt[1]==0.f)
					return lt[0]>0.f?Range(0,lt[0]):Range(lt[0],Range::Basic,lt[0],Range::Inf);
			}
			else if(hitLeft)
				return Range(0,lt[0]);
			else if(hitRight)
				return Range(0,rt[0]);
			return Range::makeInf(0,true);
		}
		else
		{		
			if(hitLeft && hitRight)
			{
				if(lt[1]>0.f && rt[1]>0.f)
					return Range(lt[0],rt[0]);
				if(lt[1]==0.f && rt[1]==0.f)
					return Range(lt[0],Range::Basic,lt[0],Range::Inf);
			}
			else if(hitLeft)
				return Range::makeInf(lt[0],true);
			else if(hitRight)
				return Range::makeInf(rt[0],true);
		}
	}
	return Range::makeZero();
}
Pose::vec getLinearVelocity(GameObject * object);
bool onRoute(Unit *unit);
Pose::vec getPathError(Unit *unit);
Pose::vec getPathTarget(Unit *unit);
float getSize(GameObject * object);

const float targetScale=1.0f;
const float pathErrorScale=0.f;

VOController::VOController(Mover* mover)
:Driver(mover)
{}

void VOController::update(float dt)
{
	typedef Pose::vec vec;
	updatePath(dt);

	Unit * unit=mover->getMaster();
	Pose currentPose = mover->getGlobalPose();
	Sphere2 bounds = unit->getBoundingSphere();
	vec position = currentPose.transformPos(bounds.center);

	float maxVelocity = mover->getMaxVelocity(0);

	segments.clear();		
	typedef Sphere2 Circle;
	Circle velocityLimits((const float*)mover->getGlobalPose().position, maxVelocity);
	for(auto it=obstacles.begin();it!=obstacles.end();++it)
	{		
		VelocityObstacle vo(position, 1.2 * bounds.radius,it->first.getPosition(0),it->second,it->first.velocity);		
		//segments|=calcSegment(obstacles[i]);
		
		addSegment(segments, velocityLimits, vo);
	}
	
	// choose best segment	
	
	float currentAngle = math3::Segment::clamp(currentPose.orientation);
	float pathAngle = currentAngle;	/// path direction
	
	float velScale = 1.f;
	// calculate segments
	if(getWaypointsCount())
	{	
		// choose best segment
		vec targ = targetScale*vec2f::normalize_s(pathDirection());
		vec path = pathErrorScale*pathError();
		vec total = path + targ;
		pathAngle=atan2(total[1],total[0]);
	}
	else if (obstacles.empty())
		velScale = 0.f;

	float bestAngle = pathAngle;

	/// no valid directions => full stop
	if(segments.isFull())
	{
		mover->directionControl(vec::zero(), 0);
		return;
	}
	auto rate = [] (float a, float b) -> float			
	{
		return 1 + cos(a)*cos(b)+sin(a)*sin(b);
	};
	/// if path direction is in the restricted area	
	if(segments.contains(pathAngle))
	{		
		math3::Segment seg(currentAngle,pathAngle);
		bestAngle = currentAngle - M_PI;
		//float bestRate = rate(prefAngle , bestAngle);
		for(auto it=segments.begin();it!=segments.end();++it)
		{		
			if(it->contains(pathAngle))
			{
				bestAngle = it->nearestBorder(currentAngle);
				bestAngle = it->nearestBorder(currentAngle);
				float r = rate(bestAngle,currentAngle);
				if(r < 1 + cosf(M_PI/2))
				{
					float test = it->nearestBorder(currentAngle);
				}
				break;
			}
/*
			auto comparator = [&] (float angle)
			{
				float d = angle - prefAngle;
				float newRate = rate(angle, prefAngle);
				if(newRate > bestRate)
				{
					bestAngle = angle;
					bestRate = newRate;
				}
			};

			comparator(it->min);
			comparator(it->max);	*/		
		}
	}	

	float r = rate(bestAngle,currentAngle);
	if(r < 1 + cosf(M_PI/2))
		int w = 0;
	//float bestAngle = 0.5 * ()
	vec vel = vec(cosf(bestAngle),sinf(bestAngle)) + 0.5 * vec(cosf(currentAngle), sinf(currentAngle));
	vel = vel.normalize() * velScale;
	float approachTime=2.0;
	float time=timeToImpact();
	if(time<approachTime && time>0)
		vel*=(time/approachTime);
	//const float t=0.5;	
	//Pose::vec newVel=mover->getDirection()*(1.f-t)+vel*t;
	mover->directionControl(vel, velScale);	
}

Mover::Driver * createVODriver(Mover* m)
{
	return new VOController(m);
}

//
void drawArc(Fx::RenderContext* rc,const float center[2],float r,const math3::Segment &seg)
{
	vec2f v(center[0],center[1]);
	const int points=18;
	drawLine(rc, v,v+r*vec2f(cosf(seg.min),sinf(seg.min)),Fx::MakeRGB(0,255,0));
	drawLine(rc, v,v+r*vec2f(cosf(seg.max),sinf(seg.max)),Fx::MakeRGB(0,255,0));
	float angle=seg.min;
	float delta=seg.length()/points;
	for(int i=0;i<points;i++)
	{
		drawLine(rc,v+r*vec2f(cosf(angle),sinf(angle)),v+r*vec2f(cosf(angle+delta),sinf(angle+delta)),Fx::MakeRGB(0,255,0));
		angle+=delta;
	}
}
//
void drawVO(Fx::RenderContext* rc,const VelocityObstacle &vo, float range, int steps)
{
	drawLine(rc, vo.apex,vo.apex+vo.dirLeft*range, Fx::MakeRGB(0,255,0));
	drawLine(rc, vo.apex,vo.apex+vo.dirRight*range,Fx::MakeRGB(0,255,0));
	float delta = range / steps;
	for(float t = delta; t < range; t+=delta)
		drawLine(rc, vo.apex+vo.dirLeft*t,vo.apex+vo.dirRight*t,Fx::MakeRGB(0,255,0));
}

void VOController::render(Fx::RenderContext* rc)
{		
	Mover::Driver::render(hge);
//	// 1. draw velocity circle
//	// 2. draw vo
	float size=20;
	Pose pose = mover->getGlobalPose();
	for(int i = 1; i < path.size(); i++)
	{
		drawLine(rc,path[i-1].v,path[i].v,RGB(255,0,0));
	}
	for(auto it=obstacles.begin();it!=obstacles.end();++it)
	{
		VelocityObstacle vo(pose.getPosition(), size, it->first.getPosition(0), it->second, it->first.velocity);		
		drawVO(rc,vo,mover->getMaster()->getSphereSize() * 5, 10);
	}
//	// 3. draw segments
	for(auto it=segments.begin();it!=segments.end();++it)
		drawArc(rc,pose.position,150*1.1,*it);
		
}

} // namespace sim
