#include "stdafx.h"
//#include "common.h"
//#include "traffic.h"
#include "Unit.h"
#include "CommandAI.h"
#include "forceController.h"
using namespace std;
typedef Unit Unit;
bool debugDrawForces=true;
bool debugDrawPath=true;
bool debugDrawObjects=true;
bool debugDrawHits=true;

bool usePrediction=true;

float privateRadius=2.0;			// greatly increase steering force from objects below this distance
float collisionRadius=1.5;			

float fPathScale[]={0.0,0.01,0.0,0},fPathK=1.0,pathSensivity=3.0;
float fEvasionScale[]={0.5,1.0,0,0},fEvasionK=3.0,fEvasionRange=5;


//float getMinDistance(const Unit &tr0,const Unit &tr1,float maxTime,float &time);
float getMinDistance(const Traectory2 &tr0,const Traectory2 &tr1,float maxTime,float &time);

// ranged random [low,high)
inline int rand(int low,int high)
{
	return std::min(low,high)+rand()%abs(high-low);
}
// random in [0,max)
inline int rand(int max)
{
	return rand()%max;
}
inline Pose::vec randVec2()
{
	return normalise(Pose::vec((float)rand(255)/255.f,
							(float)rand(255)/255.f));
}

Pose::vec calcForcePull(const Pose::vec &dir,float dist,float coeffs[4])
{
	if(dir.length()>0.f)
		return normalise(dir)*(coeffs[0]+coeffs[1]*dist+coeffs[2]*dist*dist);
	return randVec2()*coeffs[3];
}
Pose::vec calcForcePush(const Pose::vec &dir,float dist,float coeffs[4])
{
	if(dir.length()>0.f)
		return normalise(dir)/(coeffs[0]+coeffs[1]*dist+coeffs[2]*dist*dist);
	return randVec2()*coeffs[3];
}
inline float fsign(float val)
{
	return val>=0.f?1.0f:-1.f;
}
Pose::vec getLinearVelocity(GameObject * object)
{
	GameSolidObject * so=dynamic_cast<GameSolidObject * >(object);
	if(so)
		return conv(so->getBody()->GetLinearVelocity());
	return Pose::vec::zero();
	
}
bool onRoute(Unit *unit)
{
	return false;
}
Pose::vec getPathError(Unit *unit)
{
	return vec2f(0.f);
}
Pose::vec getPathTarget(Unit *unit)
{
	return vec2f(0.f);
}
float getSize(GameObject * object)
{
	return 20.f;
}
void ForceController::update(float dt)
{
	bool steering=false;
	hits.clear();
	fHit=Pose::vec::zero();
	fObjects=Pose::vec::zero();
	fPath=Pose::vec(0.f);
	float forceFactor=1.;
	Pose::vec ownerPos=owner->getPosition();
	Pose::vec ownerVel=getLinearVelocity(owner);
	Traectory2 ownerTr(ownerPos,ownerVel);
	float ownerSize=getSize(owner);//owner->size;
	float proximityRadius=ownerSize*2;
	for(auto it=objects.begin();it!=objects.end();++it)
	{
		GameObject *target=(*it);
		Pose::vec obstaclePos=target->getPosition();
		Pose::vec obstacleVel=getLinearVelocity(target);
		Traectory2 obstacleTr(obstaclePos,obstacleVel);
		float obstacleSize=getSize(target);//target->size
		
		float objDistance=vecDistance(ownerPos,target->getPosition());
		if(usePrediction)
		{
			//static Pose::vec up(0,0,1);
			float hitTime=0;
			float maxHitTime=10;
			float minDistance=getMinDistance(ownerTr,obstacleTr,5,hitTime);
			Pose::vec hit[2]={ownerPos+ownerVel*hitTime,obstaclePos+obstacleVel*hitTime};
			Pose::vec hitDelta=hit[0]-hit[1];

			//float minDistance=hitDelta.length();
			if(minDistance > (ownerSize+obstacleSize)*collisionRadius || hitTime<=0 || hitTime>=maxHitTime)
				continue;
			//steering=true;
			hits.push_back(hit[0]);
			//printf("min=%f,time=%f\n",minDistance,hitTime);			
			
			float distance=fabs(vecDistance(owner->getPosition(),hit[0]));	
			float sign=1.0f;//fsign(getVelocity()&object->getVelocity());
			if(minDistance>0.0001f)
			{
				float length=hitDelta.length()-(ownerSize+obstacleSize);
				
				fHit+=calcForcePush(hitDelta,length,fEvasionScale)*fEvasionK*sign;
			}
			else/* if(minDistance==0.f)	*/// objects are moving on equal line
			{
				printf("direct hit\n");				
				if(distance>0.f)
				{
					Pose::vec strafe=(hit[0]-ownerPos).rotate(M_PI_2);
					fHit+=calcForcePush(strafe,distance,fEvasionScale)*fEvasionK*sign;
				}
				else
				{
					Pose::vec strafe=(target->getPosition()-ownerPos).rotate(M_PI_2);
					fHit+=calcForcePush(strafe,(ownerPos-target->getPosition()).length(),fEvasionScale)*fEvasionK*sign;
				}
				//Pose::vec left=
				//float force=forceFactor*proximityRadius/distance;
				//result=result+normalise(Pose::vec(	(float)rand(255)/255.f,
												//(float)rand(255)/255.f,
												//(float)rand(255)/255.f));
			}
			
			float cd=vecLength(ownerPos-target->getPosition());
		}
		else
		{			
			float distance=fabs(objDistance-obstacleSize);
			float force=0.f;
			if(distance>collisionRadius)
				force=forceFactor*(proximityRadius-distance)/(proximityRadius-collisionRadius);
			else if(distance>0)
				force=forceFactor*proximityRadius/distance;
			float cd=vecLength(ownerPos-target->getPosition());
			if(cd>0)
			{
				fObjects=fObjects+normalise(ownerPos-target->getPosition())*force;
			}
		}	
		
	}
	
	// ForcePath
	if(!onRoute(owner) && !steering)
	{		
		Pose::vec pathError=getPathError(owner);
		if(pathError.length()>pathSensivity)
			fPath+=calcForcePull(pathError,pathError.length(),fPathScale)*fPathK;
		Pose::vec targetError=getPathTarget(owner)-owner->getPosition();
		if(targetError.length()>pathSensivity)
			fPath+=normalise(targetError)*fPathK;
	}
	// Final velocity
	/*
	if(owner->maxVelocity>0.f)
		owner->newVelocity=(owner->velocity/object->maxVelocity)+fHit+fPath+fObjects;	
	else
		owner->newVelocity=Pose::vec(0,0,0);
		*/
}

void ForceController::render()
{
	if(debugDrawForces)
	{
		float forceScale=10;
		/*
		if(debugDrawHits)
		{
			glColor3f(1,0,1);	
			drawArrow(object->position,object->position+forceScale*fHit);			
		}
		if(debugDrawObjects)
		{
			glColor3f(1,1,0);	
			drawArrow(object->position,object->position+forceScale*fObjects);
		}
		if(debugDrawPath)
		{
			glColor3f(0,1,1);	
			drawArrow(object->position,object->position+forceScale*fPath);
		}
		for(list<Pose::vec>::iterator it=hits.begin();it!=hits.end();++it)
		{
			glColor3f(0,0,0);
			drawPoint(*it,5,0);
		}
		*/
	}
}
//
//static float getMinDistance(const Unit &tr0,const Unit &tr1,float maxTime,float &time)
//{
//	float res=0;	
//	/*
//	{
//		p=s1+v1*t
//		p=s2+v2*t
//		(s1-s2)+(v1-v2)*t=0
//	} => u+v*t=0;
//	d=u*u+2*u*v*t+t*t*u*u;
//	d=a*t^2+2*b*t+c
//	dmin=c-b^2/a
//	*/
//	Pose::vec u=tr0.position-tr1.position;
//	//Pose::vec v=tr0.smoothVelocity()-tr1.smoothVelocity();
//	Pose::vec v=tr0.velocity-tr1.velocity;
//	double c=u&u;
//	double b=u&v;
//	double a=v&v;
//	if(a)
//	{
//		time=-b/a;
//		if(time>=0 &&  time<=maxTime)		
//		{
//			res=sqrt(c-b*b/a);
//		}
//		else
//		{
//			float de=a*maxTime*maxTime+b*maxTime+b*maxTime+c;
//			if(de<c)
//			{
//				time=maxTime;
//				res=sqrt(de);
//			}
//			else
//			{
//				time=0;
//				res=sqrt(c);
//			}
//		}
//	}
//	else // velocities are equal
//	{
//		res=sqrt(c);
//		time=0;
//	}
//	return res;
//}
//
//static float getMinDistance2(const Unit &tr0,const Unit &tr1,float maxTime,float &time)
//{
//	Pose::vec u=tr0.getPosition()-tr1.getPosition();
//	Pose::vec v=tr0.getVelocity()-tr1.getVelocity();
//	//Pose::vec v=tr0.velocity-tr1.velocity;
//	Pose::vec p=vecProject(u,v);
//	float distance=p.length();
//	float res=vecLength(u-p);
//	time=distance/v.length();
//
//	return res;
//}