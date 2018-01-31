#include "commandAI.h"

#include "inventory.h"
#include "mover.h"
#include "perception.h"
#include "unit.h"
#include "weapon.h"
#include "weaponTurret.h"

#include "commandAI.h"

namespace sim
{
////////////////////////////////////////////////////////////////
Command::Command(int s)
:state(s)
{}	

Command::~Command()
{}

void Command::wake(int flag)
{
	state=flag?Ready:Finished;		
}
void Command::setWait()
{
	state=Wait;
}
int Command::getState() const
{
	return state;
}

////////////////////////////////////////////////////////////////


Controller::Controller(Unit * target)
	:owner(target), mover(NULL)
{
}

Controller::~Controller()
{
	clear();
}

void Controller::update(float dt)
{
//	LogFunction(*g_logger);
	for(Container::iterator it=container.begin();it!=container.end();)
	{
		Command *command=*it;
		int state=command->process(owner);
		if(state!=Command::Process)
		{
//			g_logger->line(1,"remove task");			
			delete command;
			it=container.erase(it);
			continue;
		}
		break;
		it++;
	}
}

int Controller::invoke(Device * device,InvokerContainer::Key key,bool down,float x,float y,float wheel)
{
	if(!device)
		return 0;
	return device->onControl(key,down,x,y,wheel);
}


int Controller::onControl(InvokerContainer::Key key,bool down,float x,float y,float wheel)
{
	int result=0;
	for(auto it=owner->devices.begin();it!=owner->devices.end();++it)
		result|=invoke(*it,key,down,x,y,wheel);
	return result;
}

void Controller::render(HGE * hge)
{
	for(Container::iterator it=container.begin();it!=container.end();++it)
	{
		(*it)->render(owner,hge);
	}
}

bool Controller::add(Command *cmd)
{
//	LogFunction(*g_logger);
	bool res=cmd->check(owner);
	container.push_back(cmd);	
	return res;
}

void Controller::clear()
{
//	LogFunction(*g_logger);
	while(!container.empty())
	{
		delete container.front();
		container.erase(container.begin());
	}
}

void addSegment(Segments & segments, const Sphere2 &circle, const FlatCone &vo);

void Controller::initDevices()
{
//	LogFunction(*g_logger);
	weapons.clear();
	mover = NULL;

	for(auto it = owner->devices.begin();it!=owner->devices.end();++it)
	{
		Device * device=*it;
		Weapon * weapon = dynamic_cast<Weapon*>(device);

		if(weapon)
		{
			FlatCone cone;
			const Pose & pose = weapon->getPose();
			cone.apex = pose.getPosition();
			WeaponTurret * turret = dynamic_cast<WeaponTurret*>(weapon);
			if(turret)
			{
				vec2f dir = pose.getDirection();
				float dimensions = turret->getDimensions();
				float sn = sinf(dimensions);
				float cs = cosf(dimensions);
				cone.dirLeft = vec2f(cs,-sn);
				cone.dirRight =  vec2f(cs,sn);
			}
			else
			{
				cone.dirRight = cone.dirLeft = pose.getDirection();
			}
			weapons.push_back(std::make_pair(weapon,cone));
		}
		Mover * m=dynamic_cast<Mover*>(device);
		if(m)
		{
			mover = m;
			mover->getDriver()->listener = this;
		}
	}
	weaponReach.clear();
	Geom::_Sphere<vec2f> maxRange(vec2f(0,0), 1000);
	// calculate weapon reach regions
	for(auto it = weapons.begin(); it != weapons.end(); it++) 
	{
		addSegment(weaponReach, maxRange, it->second);		
	}
}

Command * Controller::last()
{
	if(container.empty())
		return NULL;
	return container.back();
}

void Controller::cmdMoveTo(float x,float y,float distance)
{
	CmdMove * l = dynamic_cast<CmdMove*>(last());
	if(l && !l->patrol)
		l->addPos(Pose::vec(x,y));
	else
		add(new CmdMove(Pose::pos(x,y),distance,false));
}

void Controller::cmdPatrol(float x,float y,float distance)
{
	CmdMove * l = dynamic_cast<CmdMove*>(last());
	if(l && l->patrol)
		l->addPos(Pose::vec(x,y));
	else
	{
		CmdMove * cmd = new CmdMove(owner->getPosition(),distance,true);
		cmd->addPos(Pose::pos(x,y));
		add(cmd);
	}
}

void Controller::cmdAttack(GameObjectPtr target)
{
	add(new CmdAttack(target));
}
void Controller::cmdFollow(UnitPtr target)
{
}
void Controller::cmdGuard(float x,float y,float distance)
{
	add(new CmdGuard(vec2f(x,y),distance));
}
void Controller::moverEvent(Mover *mover,int event)
{
//	LogFunction(*g_logger);
	if(!container.empty())
		return container.front()->moverEvent(mover,event);
//	else
//		g_logger->line(1, "container is empty");
}
void Controller::cmdStand(float distance)
{
	add(new CmdGuard(owner->getPosition(),distance));
}

Command * construct(CommandType type)
{
	switch(type)
	{
	case cmdAttack:
		return new CmdAttack;
	case cmdMove:
		return new CmdMove;
	case cmdGuard:
		return new CmdGuard;
	default:
		return NULL;
	}
}

void Controller::save(StreamOut & stream)
{
	unsigned char size = container.size();
	auto end = container.end();
	stream.write(size);
	for( auto it = container.begin(); it != end; ++it )
	{
		Command * cmd = *it;
		unsigned char type = cmd->getType();
		stream.write( type );
		cmd->save(stream);
	}
}

void Controller::load(StreamIn & stream)
{
	clear();
	unsigned char size = 0;
	stream.read(size);
	for(unsigned char i = 0; i < size; i++)
	{
		unsigned char type = 0;
		stream.read(type);
		Command * cmd = construct((CommandType)type);
		assert(cmd);
		cmd->load(stream);
		add(cmd);
	}
}
////////////////////////////////////////////////////////////////
CmdAttack::CmdAttack(GameObject * ptr)
	:Command(Ready),target(ptr)
{}

CmdAttack::CmdAttack(){}

int CmdAttack::process(Unit *unit)
{
	if(!target)
	{
		state = Finished;
		return Success;
	}
	if(state == Ready)
	{
		//g_logger->line(1,"CmdMove(%f,%f) - start",target[0],target[1]);
		state=Active;
		unit->controller->targets.attack = target;
		//startPos=unit->getPosition();
		//unit->startAnimation("Move");
		//unit->controller->mover->getDriver()->listener=this;
		//unit->controller->mover->getDriver()->walk(target);
		return Process;
	}
	if(state == Active)
	{
		Pose::vec pos = target->getPosition();
		if(vecDistance(unit->getPosition(),pos) > 200)
		{
			unit->controller->mover->getDriver()->walk(pos);
			attackState = Pursue;
		}
		return Process;
	}
	if(state == Finished)
	{
		//Pose::pos pos=unit->getPosition();
		//float distance=vecDistance(pos,target);
		//g_logger->line(1,"CmdMove(%f,%f) - actually in (%f,%f), %f",target[0],target[1],pos[0],pos[1],distance);			

		//unit->stopAnimation("Move");
		//unit->controller->mover->getDriver()->listener=NULL;
		//return (distance<minDist)? Success:Fail;
		return Success;
	}
	return Process;
}

void CmdAttack::moverEvent(Mover *mover,int event)
{	
//	LogFunction(*g_logger);
	if(event==Mover::Driver::taskSuccess)
	{
		
	}
	if(event==Mover::Driver::taskFail)
	{
		state=Finished;
	}	
}

void drawSolidArrow(HGE * hge, const vec2f & from, const vec2f & to,float z, DWORD color, float width, float arrowWidth, float arrowLength);

void CmdAttack::render(Unit * unit, HGE * hge)
{	
	if(target)
		drawSolidArrow(hge, unit->getPosition(), target->getPosition(), unit->getPose().position[2],RGB(0,0,255),4,8,8);
}

void CmdAttack::save(IO::StreamOut & stream)
{
	size_t id = target->id();
	stream.write(id);
}

void CmdAttack::load(IO::StreamIn & stream)
{
	size_t id = 0;
	stream.read(id);
	// TODO: target assignment by ID
}

////////////////////////////////////////////////////////////////
CmdGuard::CmdGuard(const Pose::pos &targ,float maxDistance)
	:Command(Ready),position(targ),maxDistance(maxDistance),guardState(cgsApproach)
{}

CmdGuard::CmdGuard()
{
}

void CmdGuard::moverEvent(Mover *mover,int event)
{	
//	LogFunction(*g_logger);
	if(event==Mover::Driver::taskSuccess)
	{
		if(guardState == cgsApproach)
			guardState = cgsGuard;
	}
	if(event==Mover::Driver::taskFail)
	{
		state=Finished;
		guardState = cgsGuard;
	}
}

int CmdGuard::process(Unit *unit)
{	
	if(state == Ready)
	{		
		state=Active;
		return Process;
	}
	if(state == Active)
	{		
		if(vecDistance(unit->getPosition(),position) > maxDistance)
		{
			unit->controller->mover->getDriver()->walk(position);
			guardState = cgsApproach;
		}
		return Process;
	}
	if(state==Finished)
	{
		return Success;
	}
	return Process;
}

void CmdGuard::save(StreamOut& stream)
{
	stream.write(position);
}
void CmdGuard::load(StreamIn& stream)
{
	stream.read(position);
}

const float drawTaskLineWidth = 0.1;
const float drawTaskArrowLength = drawTaskLineWidth * 6;
const float drawTaskArrowWidth = drawTaskLineWidth * 4;

const float drawTaskOwnershipLineWidth = 0.05;
const float drawTaskOwnershipArrowLength = drawTaskOwnershipLineWidth * 6;
const float drawTaskOwnershipArrowWidth = drawTaskOwnershipLineWidth * 4;

////////////////////////////////////////////////////////////////
CmdMove::CmdMove(const Pose::pos &t,float min,bool patrol)
	:Command(Ready),minDist(min),onRoute(false),finished(false),patrol(patrol),currentWaypoint(0)
{
	addPos(t);
}

CmdMove::CmdMove(){}

void CmdMove::addPos(const Pose::pos &pos)
{
	waypoints.push_back(pos);
}

void CmdMove::render(Unit * unit,HGE * hge)
{
	Pose pose = unit->getPose();
	DWORD color = hgeColorRGB(0,1,0,1).GetHWColor();
	// 1. Draw the path		
	for(int i = 1; i < waypoints.size() ; i++)
		drawSolidArrow(hge, waypoints[i-1], waypoints[i], pose.position[2], color, drawTaskLineWidth, drawTaskArrowWidth, drawTaskArrowLength);
	// 2. Loop if patrol
	if(patrol && waypoints.size())
		drawSolidArrow(hge, waypoints.back(), waypoints.front(), pose.z(), color, drawTaskLineWidth, drawTaskArrowWidth, drawTaskArrowLength);
	// 3. Draw unit relation to the task
	if( currentWaypoint >=0 && currentWaypoint < waypoints.size())
		drawSolidArrow(hge, pose.getPosition(), waypoints[currentWaypoint], pose.z(), color, drawTaskOwnershipLineWidth, drawTaskOwnershipArrowWidth, drawTaskOwnershipArrowLength);
}

void CmdMove::moverEvent(Mover *mover,int event)
{	
//	LogFunction(*g_logger);
	if(event==Mover::Driver::taskSuccess)
	{
//		g_logger->line(0,"taskSuccess");
		/// if we reached last waypoint
		if(++currentWaypoint >= waypoints.size())
		{
			
			if(patrol)
			{	
				state = Update;
				/// return on starting pos
				currentWaypoint = 0;		
//				g_logger->line(0,"last waypoint reached");
			}
			else
			{
				state=Finished;
//				g_logger->line(0,"last waypoint reached - restart path");
			}
		}
		else	/// continue moving
			state = Update;
	}
	if(event==Mover::Driver::taskFail)
	{
//		g_logger->line(0,"taskFailed");
		state=Finished;
	}	
}

int CmdMove::process(Unit *unit)
{
//	LogFunction(*g_logger);
	Pose::vec target = waypoints.front();
	if(state == Update)
	{		
		unit->controller->mover->getDriver()->walk(waypoints[currentWaypoint]);
		state = Active;
	}
	if(state == Ready)
	{
//		g_logger->line(1,"CmdMove(%f,%f) - start",target[0],target[1]);
		state = Active;
		unit->startAnimation("Move");
		unit->controller->mover->getDriver()->walk(target);
		return Process;
	}
	if(state == Finished)
	{
		Pose::pos pos = unit->getPosition();
		float distance = vecDistance(pos,target);
//		g_logger->line(1,"CmdMove(%f,%f) - actually in (%f,%f), %f",target[0],target[1],pos[0],pos[1],distance);			
		
		unit->stopAnimation("Move");
		return (distance<minDist)? Success:Fail;
	}
	return Process;
}

void CmdMove::save(IO::StreamOut & stream)
{
	stream.write(patrol);
	stream.write(currentWaypoint);
	stream.write(waypoints);
}

void CmdMove::load(IO::StreamIn & stream)
{
	stream.read(patrol);
	stream.read(currentWaypoint);
	stream.read(waypoints);
}
////////////////////////////////////////////////////////////////
CmdTakeItem::CmdTakeItem(Item *i,float dist)
	:Command(Ready),item(i),minDist(dist),moving(false)
{}

CmdTakeItem::CmdTakeItem(){}

void CmdTakeItem::moverEvent(Mover *mover,int event)
{	
	if(event==Mover::Driver::taskSuccess)
	{
		state = Finished;
		_RPT0(0,"CmdTakeItem::moverEvent() - success move\n");
	}
	if(event==Mover::Driver::taskFail)
	{
		state = Finished;
		_RPT0(0,"CmdTakeItem::moverEvent() - fail move\n");
	}	
}

bool CmdTakeItem::check(Unit *unit)
{
	if(getState()==Wait)
		return true;
	
	Pose::pos ipos = item->getPosition();
	Pose::pos upos = unit->getPosition();
	return vecDistance(ipos,upos) < minDist && item->state == Item::placeLand;
}

int CmdTakeItem::process(Unit *unit)
{
	/*	
	if(state==Ready)
	{
		_RPT0(0,"CmdTakeItem::process() - set target\n");
		if(item->state==Item::placeInventory)
			return Fail;

		unit->mover->getDriver()->listener=this;
		unit->mover->getDriver()->walk(item->getPosition());
		
		state=Active;
		return Process;
	}
	else if((state==Active || state==Finished) &&  check(unit))
	{
		_RPT0(0,"CmdTakeItem::process() - grab item\n");
		unit->inventory->addItem(item);
		item->state=Item::placeInventory;
		unit->mover->getDriver()->listener=NULL;
		return Success;
	}
	else if(state==Finished)
	{
		unit->mover->getDriver()->listener=NULL;
		return Fail;
	}
	else if(state==Active && item->state!=Item::placeLand)
	{
		_RPT0(0,"CmdTakeItem::process() - somebody already took the item\n");
		state=Finished;
		unit->mover->getDriver()->listener=NULL;
		return Fail;
	}*/
	return Process;
}

void CmdTakeItem::save(IO::StreamOut & stream)
{
}
void CmdTakeItem::load(IO::StreamIn & stream)
{
}


class PerceptionAI: public Controller
{
public:	
	typedef Vector<float,typeMax> Weights;
	Weights weights;
	
	TurretDriver turretDriver;

	PerceptionAI(Unit * unit,const Weights &base)
		:Controller(unit),weights(base)
	{
		weapons.reserve(16);
	}	

	void update(float dt)
	{
		Controller::update(dt);
		Mover::Driver * driver=NULL;
		if(mover)
		{
			driver = mover->getDriver();
			driver->obstaclesClear();
		}
		//if(container.empty())
		{
			
			PerceptionClient::Units hostile;
			//owner->perception->getHostile(hostile);
			PerceptionClient::Items items;
			/// enum hostile objects
			for(auto it = objects.begin(); it != objects.end();++it)
			{        
				GameObject * object=*it;
				if(driver && object)
					driver->obstaclesAdd(getTraectory2(object),object->getBoundingSphere().radius);
				Unit * u=dynamic_cast<Unit*>(object);
				if(u && u->player!=owner->player)
				{
					hostile.push_back(u);
				}
				//if(object->pl
			}
			//owner->perception->getItems(items);
			//float danger=0;
			//for(Perception::Units::iterator it=hostile.begin();it!=hostile.end();++it)
			//	danger+=rateDanger(*it);
			if(!targets.attack && !hostile.empty())	// if have weapons - attack
			{
				float minRange=10000;
				Unit * nearestEnemy=NULL;
				/// find the nearest enemy
				for(auto it = hostile.begin();it!=hostile.end();++it)
				{
					Unit * obj=*it;
					if(obj->isDead())
						continue;
					float range=vecDistance(obj->getPosition(),owner->getPosition());
					if(range<minRange)
					{
						minRange=range;
						nearestEnemy=obj;
					}
				}
				if(nearestEnemy)	
					targets.attack=nearestEnemy;						
				else
				{
					for(int i = 0; i < weapons.size(); i++)
					{
						WeaponTurret * turret=dynamic_cast<WeaponTurret*>(weapons[i].first);
						if(!turret)continue;
					}	
				}
			}
			if(!items.empty())
			{
				_RPT0(0,"taking an item\n");
				add(new CmdTakeItem(items.front(),10));
			}
		}	
		updateAttack(dt);
	}


	void updateAttack(float dt)
	{
		if(!targets.attack)
			return;
		float distance = vecDistance(owner->getPosition(),targets.attack->getPosition());
		TrackingInfo info;
		info.object = targets.attack;

		info.pos = targets.attack->getPose().transformPos( targets.attack->getBoundingSphere().center);
		info.vel = vec2f(0,0);//targets.attack->getBody()->GetLinearVelocity();
		info.size = targets.attack->getBoundingSphere().radius;
		for(int i = 0;i < weapons.size(); i++)
		{
			WeaponTurret * turret=dynamic_cast<WeaponTurret*>(weapons[i].first);
			if(!turret)
				continue;
			if(turretDriver.aim(turret,info,dt))
				turret->shoot();
			//TurretDriver * driver=dynamic_cast<TurretDriver*>(turret->autoDriver());
			//if(!driver)continue;
			//driver->aim(target->getPosition());
		}	
	}

	/*void attack(Unit * target)
	{		
		for(int i=0;i<weapons.size();i++)
		{
			//if(!unit->weapons[i]) continue;
			WeaponTurret * turret=dynamic_cast<WeaponTurret*>(weapons[i]);
			if(!turret)continue;
			turretDriver.update(turret,target->getPosition(),dt);
			//TurretDriver * driver=dynamic_cast<TurretDriver*>(turret->autoDriver());
			if(!driver)continue;
			driver->aim(target->getPosition());
		}		
	}*/
	bool hasWeapons();

	float rateDanger(Unit * target)
	{
		return target->stats[Hitpoints]/(owner->stats[Hitpoints]+1);
	}
	float rateItem(Item *item)
	{
		if(item->localDef().itemType=="Food" && owner->stats[Hitpoints]<20)
			return 10.0;
		return 1.0;
	}
};
bool PerceptionAI::hasWeapons()
{
	/*for(int i=0;i<owner->weapons.size();i++)
		if(owner->weapons[i])
			return true;*/
	return false;
}
Controller * createPerceptionAI(Unit *u)
{
	return new PerceptionAI(u,PerceptionAI::Weights(1.0f));
}
Controller * createCommandAI(Unit * u)
{
	return new Controller(u);
}

}
