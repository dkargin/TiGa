#include "stdafx.h"

//#include "globals.h"
#include "perception.h"
#include "moverVehicle.h"

#include "inventory.h"
#include "weapon.h"
#include "projectile.h"
#include "unit.h"
#include "commandAI.h"

#include "weaponTurret.h"
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

typedef pathProject::TraectoryLinear Traectory;
typedef std::list<Geom::Traectory2 *> Traectories;
template<class Container> void clear(Container &container)
{
	for(Container::iterator it=container.begin();it!=container.end();it++)
		delete *it;
	container.clear();	
}
// solve square equation a*x^2+b*x+c=0
template<class Real> int solve(const Real &a,const Real &b,const Real &c,Real result[2])
{	
	if(a!=Real(0))
	{
		Real D = b*b - 4*a*c;
		if(D<Real(0))
			return 0;
		else if(D == Real(0.0f))
		{
			result[0]=-b/(2*a);
			return 1;
		}
		else
		{
			D=sqrt(D);
			result[0] = (-b-D)/(2*a);
			result[1] = (-b+D)/(2*a);
			if(result[0] > result[1])
			{
				Real tmp = result[0];
				result[0] = result[1];
				result[1] = tmp;
			}
			return 2;
		}
	}
	else
	{
		if(b != Real(0))
		{
			result[0] =- c/b;
			return 1;
		}
		return 0;
	}
}

// calc aiming target
// рассчитываем, в каком направлении (в какую точку) надо выстрелить снар€дом, перемещающимс€ с посто€нной скоростью,
// чтобы попасть в цель, перемещающуюс€ так же с посто€нной скоростью
vec3 getWeaponTarget(const Traectory &target, const vec3 &pos, ProjectileDef *def)
{
	vec3 result = target(0);
	if(def->projectileType == projectileDirect)
	{		
		// проекци€ исходного положени€ на траекторию
		Geom::Edge edge(target(0),target(1));
		vec3 O = edge.project(pos);
		// вектор, перпендикул€рный траектории
		vec3 H = O-pos;
		float h = H.length();
		float l = edge.projectLen(pos);
		float impactTime[2] = {0,0};
		float v0 = target.velocity.length();	// скорость объекта который надо сбить
		float v1 = def->velocity;				// скорость снар€да которым сбиваем
		int res = solve(v0*v0-v1*v1,-2*l*v0,h*h+l*l,impactTime);
		if(res>0)
		{
			float time=impactTime[res-1];		// если 2 корн€, берЄм impactTime[1], если один корень - impactTime[0]
			float vx = h;						                                            // скорость в направлении, перпендикул€рном траектории
			float vy = v0*time-l;				// скорость в направлении, параллельном траектории
			result = pos + (normalise(H)*vx + edge.direction()*vy);
		}
	}
	else if(def->projectileType==projectileBallistic)
	{}
	return result;
}

// calculate aiming position to hit moving object
vec2f getWeaponTarget2(const Geom::Traectory2 &target,const vec2f &pos,ProjectileDef *def)
{
	typedef Geom::_Edge<vec2f> Edge2;
	vec2f result=target(0);
	if(def->projectileType==projectileDirect)
	{		
		// проекци€ исходного положени€ на траекторию
		Edge2 edge(target(0),target(1));
		vec2f O = edge.project(pos);
		// вектор, перпендикул€рный траектории
		vec2f H = O-pos;
		float h = H.length();
		float l = edge.projectLen(pos);
		float impactTime[2] = {0,0};
		float v0 = target.velocity.length();	// скорость объекта который надо сбить
		float v1 = def->velocity;				// скорость снар€да которым сбиваем
		int res = solve(v0*v0 - v1*v1,-2*l*v0, h*h + l*l,impactTime);
		if(res>0)
		{
			float time = impactTime[res-1];	// если 2 корн€, берЄм impactTime[1], если один корень - impactTime[0]
			float vx = h;						// скорость в направлении, перпендикул€рном траектории
			float vy = v0*time-l;				// скорость в направлении, параллельном траектории
			result = pos + (normalise(H)*vx + edge.direction()*vy);
		}
	}
	else if(def->projectileType==projectileBallistic)
	{}
	return result;
}

// calculate aiming position to hit moving object
vec2f getWeaponTarget2(const TrackingInfo &info,const vec2f &pos,ProjectileDef *def)
{
	typedef Geom::_Edge<vec2f> Edge2;
	vec2f result = info.pos;
	if(def->projectileType==projectileDirect)
	{		
		// проекци€ исходного положени€ на траекторию
		Edge2 edge(info.pos,info.pos + info.vel);
		vec2f O = edge.project(pos);
		// вектор, перпендикул€рный траектории
		vec2f H = O-pos;
		float h = H.length();
		float l = edge.projectLen(pos);
		float impactTime[2] = {0,0};
		float v0 = info.vel.length();	// скорость объекта который надо сбить
		float v1 = def->velocity;				// скорость снар€да которым сбиваем
		int res = solve(v0*v0 - v1*v1,-2*l*v0, h*h + l*l,impactTime);
		if(res>0)
		{
			float time = impactTime[res-1];	// если 2 корн€, берЄм impactTime[1], если один корень - impactTime[0]
			float vx = h;						// скорость в направлении, перпендикул€рном траектории
			float vy = v0*time-l;				// скорость в направлении, параллельном траектории
			result = pos + (normalise(H)*vx + edge.direction()*vy);
		}
	}
	else if(def->projectileType==projectileBallistic)
	{}
	return result;
}
//
//class PerceptionAI: public CommandAI
//{
//public:	
//	enum
//	{
//		typeAttack,
//		typeHeal,
//		typeWork,
//		typeObey,
//		typeMax
//	};
//	typedef Vector<float,typeMax> Weights;
//	Weights weights;
//	//Traectories traectories;
//	MoverVehicle::Driver * driver;	// mover driver - controls all movement tasks, like path following, evasion, pursuing
//	PerceptionAI(Unit * unit,const Weights &base)
//		:CommandAI(unit),weights(base)
//	{
//		driver=dynamic_cast<MoverVehicle::Driver *>(owner->mover->getDriver());
//	}
//	void process(float dt)
//	{	
//		if(driver)
//			driver->obstaclesClear();
//		//::clear(traectories);
//
//		//if(container.empty())
//		{
//			Perception::Units hostile;
//			perception()->getHostile(hostile);
//			Perception::Objects projectiles;
//			perception()->getProjectiles(projectiles);
//			Perception::Items items;
//			perception()->getItems(items);
//
//			
//			evade(hostile);
//			evade(projectiles);
//			
//			//float danger=0;
//			//for(Perception::Units::iterator it=hostile.begin();it!=hostile.end();it++)
//			//	danger+=rateDanger(*it);
//			if(!hostile.empty())	// if have weapons - attack
//			{
//				float minRange=10000;
//				Unit * nearestEnemy=NULL;
//				for(Perception::Units::iterator it=hostile.begin();it!=hostile.end();it++)
//				{
//					Unit * unit=*it;
//					if(unit->isDead())
//						continue;					
//					
//					float range=vecDistance(unit->getPosition(),owner->getPosition());
//					if(range<minRange)
//					{
//						minRange=range;
//						nearestEnemy=unit;
//					}
//				}
//				if(nearestEnemy)
//				{
//					for(int i=0;i<owner->weapons.size();i++)
//					{
//						if(!owner->weapons[i]) continue;
//						WeaponTurret * turret=dynamic_cast<WeaponTurret*>(owner->weapons[i]);
//						if(!turret)continue;
//						TurretDriver * driver=dynamic_cast<TurretDriver*>(turret->autoDriver());
//						if(!driver)continue; 
//						Pose::pos target=(const float*)getWeaponTarget2(getTraectory2(nearestEnemy),owner->getPosition(),turret->getDefinition()->projectile);
//						driver->aim(target);
//					}
//				}
//
//			}
//			if(!items.empty())
//			{
//				_RPT0(0,"taking an item\n");
//				add(new CmdTakeItem(items.front(),10));
//			}
//		}
//		
//		CommandAI::process(dt);
//	}
//	bool hasWeapons();
//
//	float rateDanger(Unit * target)
//	{
//		return target->stats[Hitpoints]/(owner->stats[Hitpoints]+1);
//	}
//	float rateItem(Item *item)
//	{
//		if(item->definition->itemType=="Food" && owner->stats[Hitpoints]<20)
//			return 10.0;
//		return 1.0;
//	}
//	// send evade list to mover driver. Returns number of new evading tasks
//	template<class Container> int evade(Container &c)
//	{
//		int i=0;
//		if(!driver)
//			return i;
//		for(Container::iterator it=c.begin();it!=c.end();it++)
//		{
//			if((*it)->isDead())
//				continue;
//			//traectories.push_front(new Traectory(getTraectory(*it))); 
//			driver->obstaclesAdd(getTraectory2(*it),40);
//			i++;
//		}
//		return i;
//	}
//	Perception * perception()
//	{
//		return owner->perception;
//	}
//};
//bool PerceptionAI::hasWeapons()
//{
//	for(int i=0;i<owner->weapons.size();i++)
//		if(owner->weapons[i])
//			return true;
//	return false;
//}
//CommandAI * createPerceptionAI(Unit *u)
//{
//	return new PerceptionAI(u,PerceptionAI::Weights(1.0f));
//}