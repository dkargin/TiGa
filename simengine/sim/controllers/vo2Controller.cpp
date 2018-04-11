#include "vo2Controller.h"

#include "sim/device.h"
#include "sim/mover.h"
#include "sim/moverVehicle.h"
#include "sim/unit.h"

namespace sim
{

const int steps = 128;
const float delta=(2*M_PI)/steps;

const float targetScale=2.0f;
const float pathErrorScale=0.1f;

// ��������� ������ � VelocityObstacles
void VO2Controller::updateObstacles(int safetyLevel)
{
	Unit* unit=mover->getMaster();
	vec2f size=unit->getOOBB().size();
	float sphereSize=1.3*unit->getBoundingSphere().radius;
	velocitySpace.resize(0);
	velocitySpace.reserve(obstacles.size());

	auto checkObstacle = [&](const Obstacle & obstacle)
	{
		vec2f dir = obstacle.first.getPosition(0) - mover->getGlobalPose().getPosition();
		vec2f relativeVelocity = -obstacle.first.velocity + conv(mover->getMaster()->getBody()->GetLinearVelocity());
		bool add = true;
		if(safetyLevel)
		{
			float len2 = dir.length_squared();			
			float vel = dir & relativeVelocity;
			int safetyScale = 1;
			if( vel <= 0.f )
				safetyScale  = 2;
			
			float timeToReach = fabs(len2 / vel);
			float timeToEvade = safetyLevel * sphereSize / (mover->getMaxVelocity(0) * safetyScale);
			add =  timeToEvade > timeToReach;			
		}
		if(add)
			velocitySpace.push_back(VelocityObstacle(vec2f::zero(),sphereSize,dir,obstacle.second,obstacle.first.velocity));
	};

	std::for_each(obstacles.begin(), obstacles.end(), checkObstacle);
}

// ���������� ����������� ����� �� ����� VO
void rayCastFan(const std::vector<VO2> &vo,float startAngle,int steps,Rays &rays)
{
	rays.resize(steps);
	float delta=2*M_PI/steps;
	unsigned int i=0;
	float cs = cosf(delta);
	float sn = sinf(delta);

	vec2f ray(cosf(startAngle), sinf(startAngle));

	for(float angle = startAngle; i < steps; angle += delta, i++)
	{
		//float maxDistance=object->maxVelocity;
		float x1 = ray[0] * cs - ray[1] * sn;
		float y1 = ray[0] * sn + ray[1] * cs;

		ray[0] = x1;
		ray[1] = y1;

		RangeSet &rs=rays[i].second;
		rs.clear();
		// ���� ����������� �� ����� VO, �������� ����� �������
		for(unsigned int obs=0;obs<vo.size();obs++)
		{
			rs |= vo[obs].rayCast(ray);
		}
		rays[i].first=angle;		
	}
}

void getMaxRanges(Rays &rays,float maxRange,std::vector<float> &distance)
{
	if(distance.size()!=rays.size())
		distance.resize(rays.size(),0);
	/*
	������� ������ �� ����������� [0,maxVelocity],
	����� ���� ��������� ��� ���������� ����������, � �������
	������������ �������.
	*/
	for(unsigned int i=0;i<rays.size();i++)
	{
		float angle = rays[i].first;
		const RangeSet &rs = rays[i].second;
		float &bestDistance = distance[i];
		//// ���� �� ����������� ������� ���������� ��������
		//if(rs.isInf())// ���� �� ����������� (��������� �������) ��������� � �������������
		//{
		//	for(RangeSet::Ranges::const_iterator it=rs.ranges.begin();it!=rs.ranges.end();++it)
		//	{
		//		const Range &r=*it;				
		//		if(r.isInf() && r.min>maxRange)	// 
		//			break;				
		//		if(r.max>bestDistance)
		//			bestDistance=r.max;
		//		else if(r.min>bestDistance)
		//			bestDistance=r.min;
		//	}
		//}
		if(rs.isZero())// ������ ��������� - ����������� ��, �������� ������������ ��������.
			bestDistance = maxRange;
		else		// �������� ���������, � ���� �� ����� �������. ����� ���� ���������� ��������, 
					// ������ ����� �������� ��������� ����� �� ��� � � ������ ������������ ���������
		{
			bool max=false;
			for(RangeSet::Ranges::const_iterator it=rs.ranges.begin();it!=rs.ranges.end();++it)
			{
				const Range &r=*it;		
//				if(r.isZero())
//					continue;
				if(r.contains(maxRange))	// 
				{
					bestDistance=r.min;
					max=true;
					break;				
				}
				if(r.max>bestDistance)
					bestDistance=r.max;
				else if(r.min>bestDistance)
					bestDistance=r.min;
			}
			if(!max)
				bestDistance=maxRange;
		}
		// ����� �� ������� ��� �������� � ���������������� � ������� ���������.
		// � ������ ������ � �������� ���� �������������� ����� �������� �������� 
		// ���������������� � ������� ���������
	}
}

// �������� ����� ���������� ��������� ���������
void getMinRanges(Rays &rays,float minRange,std::vector<float> &distance)
{
	if(distance.size()!=rays.size())
		distance.resize(rays.size(),0);
	/*
	������� ������ �� ����������� [0,maxVelocity],
	����� ���� ��������� ��� ���������� ����������, � �������
	������������ �������.
	*/
	for(size_t i = 0; i < rays.size(); i++)
	{
		float angle = rays[i].first;
		const RangeSet &rs = rays[i].second;
		float &bestDistance = distance[i];
		
		if(rs.isZero())	// ������ ��������� - ����������� ���, �������� ������� ��������.
			bestDistance = minRange;
		else			// �������� ���������, � ���� �� ����� �������. ����� ���� ���������� ��������, 
						// ������ ����� �������� ��������� ����� �� ��� � � ������ ������������ ���������
		{
			bool min = false;
			const Range &r = rs.ranges.front();
			if( r.contains(minRange) && r.contains(0) )
			{
				bestDistance = -1;
			}
			else
			{
				bestDistance = r.min;
			}
		}
		// ����� �� ������� ��� �������� � ���������������� � ������� ���������.
		// � ������ ������ � �������� ���� �������������� ����� �������� �������� 
		// ���������������� � ������� ���������
	}
}

void VO2Controller::update(float dt)
{
	updatePath(dt);
	updateObstacles(0);

	float maxVelocity = mover->getMaxVelocity(0);
	// ����������, ��� ������ �������� �� ���������� ��������� � 
	// �������� ��������������� ��������� ��������
	//Pose::vec targetDir = this->pathDirection();
	//float distance=targetDir.length();
	//const float targetCloseFactor = 0.2f;
	//vec2f size = mover->getUnit()->getOOBB().size();
	//float sphereSize=1.3 * mover->getUnit()->getBoundingSphere().radius;
	float turningSpeed = mover->getMaxVelocity(1);
	float turningRadius = maxVelocity / turningSpeed;
	Pose moverPose = mover->getGlobalPose();
	Pose::vec velocity = Pose::vec::zero();
	Pose::vec preferedDirection = Pose::vec::zero();
	
	float preferedAngle = 0;
	float preferedSpeed = 0;
	bool findMax = true;
	bool turnOnly = false;
	if( pathCurrent != -1 )
	{
		Pose::vec pathDir = pathDirection();
		Pose::vec targ = targetScale * pathDir;
		Pose::vec path = pathErrorScale * pathError();

		if( fabs( pathDir[0] ) < 2 * turningRadius && fabs( pathDir[1] ) < 2*turningRadius)
		{
			float x = pathDir & moverPose.axisX();
			float y = fabs(pathDir & moverPose.axisY()) - turningRadius;
			turnOnly = ( x*x  + y*y < turningRadius*turningRadius );
		}

		preferedDirection = (path + targ).normalize();
		preferedAngle = atan2(preferedDirection[1], preferedDirection[0]);	// � �� ����� �� ��� ��� ����
		preferedSpeed = maxVelocity;										// � ���������
		MoverVehicle * mv = (MoverVehicle*)mover;
		if(mv->kinematic)
		{
			Pose::vec bodyVelocityLinear = conv(mover->getBody()->GetLinearVelocity());
			float bodyVelocityAngular = mover->getBody()->GetAngularVelocity();
			/*
			velProj = (vel & pathDir) / |pathDir|
			time = |pathDir| / |velProj| = |pathDir| * |pathDir| / (vel & pathDir)
			*/
			//float distanceLeft = bodyVelocityLinear & pathDir.normalise();
			/*
			float arriveTime = pathDir.length_squared() / (bodyVelocityLinear & pathDir);
			float brakeTime = bodyVelocityLinear.length() / mv->definition->acceleration[0];
			if(arriveTime > 0 && arriveTime < brakeTime)
			{
				preferedSpeed = arriveTime * mv->definition->acceleration[0];
				if(preferedSpeed > maxVelocity) 
					preferedSpeed = maxVelocity;
			}*/
		}
	}
	else
	{
		findMax = false;
		preferedSpeed = 0;													// �������� �� ������ �� ����� � �� ���������
		preferedAngle = mover->getGlobalPose().orientation;					// � �������� � �� �� �������		
	}

	float bestRate = 2 * maxVelocity;				// ������ �������, ��� ������ ���� ����� ������
	Pose::vec bestVelocity = Pose::vec::zero();		// ������ ��������
	// choose best segment
	
	rayCastFan(velocitySpace,preferedAngle,steps,rays);

	/*
	������� ������ �� ����������� [0,maxVelocity],
	����� ���� ��������� ��� ���������� ����������, � �������
	������������ �������.
	*/
	std::vector<float> bestRanges;
	if(findMax)
		getMaxRanges(rays, preferedSpeed, bestRanges);
	else
		getMinRanges(rays, 0, bestRanges);
	/*
	����� �������� ��������� �����������
	*/
	vec2f preferedVelocity = preferedDirection * preferedSpeed;
	for(int i = 0; i < rays.size();i++)
	{
		float angle = rays[i].first;
		float distance = bestRanges[i];
		if( distance < 0 )
			continue;
		Pose::vec vel = distance * Pose::vec(cosf(angle),sinf(angle));
		float rate = vecDistance(vel, preferedVelocity);
		if(distance > 0 && rate < bestRate)
		{
			bestRate = rate;
			bestVelocity = vel;
		}
	}	

	velocity = bestVelocity;

	float approachTime=2.0;
	float time = timeToImpact();
	if(time<approachTime && time>0)
		velocity*=(time/approachTime);

	const float t = 0.5;	// ��������� ���������� �������. 
	mover->directionControl(velocity, turnOnly ? 0.f : velocity.length());
	//object->newVelocity=object->velocity*(1.f-t)+velocity*t;
}

Mover::Driver * createVO2Driver(Mover* m)
{
	return new VO2Controller(m);
}
// ������ ������ ���. ������ ����� ��������- ����������� � VO
void drawRangeSet(Fx::RenderContext* rc, const vec2f & pos, const RangeSet &rs,float angle)
{
	float cs=cosf(angle),sn=sinf(angle);

	Fx::FxRawColor color = Fx::MakeRGB(255, 255, 255);
	Fx::VertexBatch batch(Fx::VertexBatch::PRIM_LINES);

	const float veryFar = 60.f;

	for(RangeSet::Ranges::const_iterator it=rs.ranges.begin();it!=rs.ranges.end();++it)
	{
		const Range &r=*it;
		if(!r.isInf())
			batch += {
				Fx::Vertex::make2c(pos[0] + r.min * cs, pos[1] + r.min * sn, color),
				Fx::Vertex::make2c(pos[0] + r.max * cs, pos[1] + r.max * sn, color)
			};
		else if(r.tmax==Range::Inf)
			batch += {
					Fx::Vertex::make2c(pos[0] + r.min * cs, pos[1] + r.min * sn, color),
					Fx::Vertex::make2c(pos[0] + veryFar * cs, pos[1] + veryFar * sn, color)
			};
	}

	rc->renderBatch(batch);
}

// ������ ��������� �����
void drawRays(Fx::RenderContext* rc, const vec2f &pos,const Rays &rays)
{
	for(unsigned int i=0;i<rays.size();i++)
	{
		float angle=rays[i].first;
		drawRangeSet(rc, pos, rays[i].second, angle);
	}
}

//
//template<> VO2Controller * operator dynamic_cast<VO2Controller>(Mover::Driver * driver)
//{
//	return NULL;
//}

void VO2Controller::render(Fx::RenderContext* rc)
{		
	Mover::Driver::render(rc);
	float size = 5 * mover->getMaster()->getSphereSize();
	Pose pose = mover->getGlobalPose();
	for(auto it=obstacles.begin();it!=obstacles.end();++it)
	{
		VelocityObstacle vo(pose.getPosition(),size,it->first.getPosition(0),it->second,it->first.velocity);		
		drawVO(rc, vo, size, 10);
	}

	drawRays(rc, pose.getPosition(), rays);
//	
//	//for(auto it=segments.begin();it!=segments.end();++it)
//	//	drawArc(object->position,object->maxVelocity*1.1,*it);	
}
}
