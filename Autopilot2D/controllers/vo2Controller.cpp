#include "stdafx.h"
#include "device.h"
#include "mover.h"
#include "unit.h"
#include "moverVehicle.h"
#include "vo2Controller.h"

const int steps = 128;
const float delta=(2*M_PI)/steps;

const float targetScale=2.0f;
const float pathErrorScale=0.1f;

// Заполняем массив с VelocityObstacles
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

// определяем пересечения лучей со всеми VO
void rayCast(const std::vector<VO2> &vo,float startAngle,int steps,Rays &rays)
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
		// ищем пересечение со всеми VO, выбираем самое дальнее
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
	сначала отсечь всё промежутком [0,maxVelocity],
	затем надо перебрать все оставшиеся промежутки, и выбрать
	максимальную границу.
	*/
	for(unsigned int i=0;i<rays.size();i++)
	{
		float angle = rays[i].first;
		const RangeSet &rs = rays[i].second;
		float &bestDistance = distance[i];
		//// надо из промежутков выбрать подходящую скорость
		//if(rs.isInf())// один из промежуктов (последний вестимо) упирается в бесконечность
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
		if(rs.isZero())// пустое множество - препятствий не, выбираем максимальную скорость.
			bestDistance = maxRange;
		else		// непустое множество, и даже не очень длинное. Снова ищем подходащую скорость, 
					// скорее всего алгоритм получится таким же как и в случае бесконечного множества
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
		// затем мы сравним эту скорость с предпочтительной и выберем наилучшую.
		// в данный момент в качестве меры предполагается длина разности векторов 
		// предпочтительной и текущей скоростей
	}
}

// получаем набор минимально возможных скоростей
void getMinRanges(Rays &rays,float minRange,std::vector<float> &distance)
{
	if(distance.size()!=rays.size())
		distance.resize(rays.size(),0);
	/*
	сначала отсечь всё промежутком [0,maxVelocity],
	затем надо перебрать все оставшиеся промежутки, и выбрать
	максимальную границу.
	*/
	for(size_t i = 0; i < rays.size(); i++)
	{
		float angle = rays[i].first;
		const RangeSet &rs = rays[i].second;
		float &bestDistance = distance[i];
		
		if(rs.isZero())	// пустое множество - препятствий нет, выбираем заданую скорость.
			bestDistance = minRange;
		else			// непустое множество, и даже не очень длинное. Снова ищем подходащую скорость, 
						// скорее всего алгоритм получится таким же как и в случае бесконечного множества
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
		// затем мы сравним эту скорость с предпочтительной и выберем наилучшую.
		// в данный момент в качестве меры предполагается длина разности векторов 
		// предпочтительной и текущей скоростей
	}
}

void VO2Controller::update(float dt)
{
	updatePath(dt);
	updateObstacles(0);

	float maxVelocity = mover->getMaxVelocity(0);
	// Определяем, как далеко осталось до ближайшего вейпоинта и 
	// выбираем соответствующую стратегию движения
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

		preferedDirection = normalise(path + targ);
		preferedAngle = atan2(preferedDirection[1], preferedDirection[0]);	// а не пойти ли нам вон туда
		preferedSpeed = maxVelocity;										// и побыстрее
		MoverVehicle * mv = (MoverVehicle*)mover;
		if(mv->definition->kinematic)
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
		preferedSpeed = 0;													// хотелось бы стоять на месте и не двигаться
		preferedAngle = mover->getGlobalPose().orientation;					// и смотреть в ту же сторону		
	}

	float bestRate = 2 * maxVelocity;				// лучший рейтинг, для начала берём самый худший
	Pose::vec bestVelocity = Pose::vec::zero();		// лучшая скорость
	// choose best segment
	
	::rayCast(velocitySpace,preferedAngle,steps,rays);

	/*
	сначала отсечь всё промежутком [0,maxVelocity],
	затем надо перебрать все оставшиеся промежутки, и выбрать
	максимальную границу.
	*/
	std::vector<float> bestRanges;
	if(findMax)
		getMaxRanges(rays, preferedSpeed, bestRanges);
	else
		getMinRanges(rays, 0, bestRanges);
	/*
	затем выбираем наилучшее направление
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

	const float t = 0.5;	// варьируем инертность объекта. 
	mover->directionControl(velocity, turnOnly ? 0.f : velocity.length());
	//object->newVelocity=object->velocity*(1.f-t)+velocity*t;
}

Mover::Driver * createVO2Driver(Mover* m)
{
	return new VO2Controller(m);
}
// рисуем каждый луч. Точнее набор отрезков- пересечений с VO
void drawRangeSet(HGE * hge, const vec2f & pos, const RangeSet &rs,float angle)
{
	float cs=cosf(angle),sn=sinf(angle);
	//glBegin(GL_LINES);
	for(RangeSet::Ranges::const_iterator it=rs.ranges.begin();it!=rs.ranges.end();++it)
	{
		const Range &r=*it;
		if(!r.isInf())		
		{
			hge->Gfx_RenderLine(pos[0] + r.min * cs, pos[1] + r.min * sn, pos[0] + r.max * cs, pos[1] + r.max * sn);
		}
		else if(r.tmax==Range::Inf)
		{
			const float veryFar = 60.f;
			hge->Gfx_RenderLine(pos[0] + r.min * cs, pos[1] + r.min * sn, pos[0] + veryFar * cs, pos[1] + veryFar * sn);
		}
	}
}

// рисуем множество лучей
void drawRays(HGE * hge, const vec2f &pos,const Rays &rays)
{
	for(unsigned int i=0;i<rays.size();i++)
	{		
		float angle=rays[i].first;
		drawRangeSet(hge, pos, rays[i].second, angle);
	}
}

//
//template<> VO2Controller * operator dynamic_cast<VO2Controller>(Mover::Driver * driver)
//{
//	return NULL;
//}

void VO2Controller::render(HGE * hge)
{		
	Mover::Driver::render(hge);
	float size = 5 * mover->getMaster()->getSphereSize();
	Pose pose = mover->getGlobalPose();
	for(auto it=obstacles.begin();it!=obstacles.end();++it)
	{
		VelocityObstacle vo(pose.getPosition(),size,it->first.getPosition(0),it->second,it->first.velocity);		
		drawVO(hge,vo, size, 10);
	}

	drawRays(hge, pose.getPosition(), rays);
//	
//	//for(auto it=segments.begin();it!=segments.end();++it)
//	//	drawArc(object->position,object->maxVelocity*1.1,*it);	
}