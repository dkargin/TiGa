#pragma once
#include "frostTools/mathSet.h"
#include "mover.h"
#include "perception.h"

enum CommandType
{
	cmdUnknown,
	cmdAttack,				// attack some place
	cmdAttackObject,
	cmdMove,
	cmdGuard,
	cmdPatrol,
	cmdEvade,
	cmdTakeItem,
	cmdUseItem,
	cmdDropItem,
};

class Unit;

/// Base class for any unit command
class Command
{
	friend class CommandAI;
public:
	enum CommandState
	{
		Wait,	// wait when other task is finished
		Ready,
		Update,
		Active,
		Finished
	};	
	enum Result
	{
		Fail=0,
		Success,
		Process,
	};

	Command(int s=Ready);
	virtual ~Command();

	void wake(int flag);
	virtual void setWait();
	virtual int process(Unit *unit)=0;
	virtual bool check(Unit *unit){return true;}	// is it possible to execute command

	virtual int getState() const;
	virtual int getType() const {return cmdUnknown;}
	virtual void render(Unit * unit,HGE * hge) {};
	/// mover events callback
	virtual void moverEvent(Mover *mover,int event){};
	virtual void save(IO::StreamOut & stream) = 0;
	virtual void load(IO::StreamIn & stream) = 0;
protected:
	int state;
};

class Weapon;
class Mover;

struct Command2;
/// теперь пытаемся затолкать все команды в одну структуру
typedef int (*CmdProc)(Command2 &cmd, Controller &controller);
struct Command2
{
	int type;
	int state;
	CmdProc cmdProc;
	Pose::vec target;
};

typedef _Range<float> Range;
typedef _RangeSet<Range> RangeSet;
typedef std::vector<std::pair<float,RangeSet>> Rays;

struct FlatCone
{
	typedef Pose::vec vec;
	Pose::vec apex;
	Pose::vec dirLeft,dirRight;	

	float distanceLeft, distanceRight;

	enum Flags
	{
		zeroApex = 1,
		zeroInside = 2,
	};

	int flags;

	FlatCone():apex(vec::zero()),dirLeft(vec::zero()),dirRight(vec::zero()),flags(zeroApex),distanceLeft(0.f),distanceRight(0.f){}
	FlatCone(const Pose::vec& posA,float sizeA,const Pose::vec& posB,float sizeB,const Pose::vec& vel);
	FlatCone(const FlatCone & vo):apex(vo.apex), dirLeft(vo.dirLeft), dirRight(vo.dirRight),flags(vo.flags),distanceLeft(vo.distanceLeft),distanceRight(vo.distanceRight) {}

	Range rayCast(const vec2f & ray)const;

	
	int intersectionLeft(const vec2f &ray, float res_t[2]) const;
	int intersectionRight(const vec2f &ray, float res_t[2]) const;
};
/// Executes command queue and handles events from devices
class Controller: public PerceptionClient, public Mover::Driver::Listener//, public Assembly::Listener
{
protected:
	typedef std::vector<Command*> Container;
	Container container;
public:
	std::vector<std::pair<Weapon*, FlatCone>> weapons;
	Segments weaponReach;
	Mover * mover;
	
	Unit *owner;

	struct
	{
		GameObjectPtr attack;	/// someone to focus
		Pose::pos aim;			/// point to aim
		GameObjectPtr follow;	/// someone to follow
	}targets;
	
	Controller(Unit * target);
	virtual ~Controller();

	static int invoke(Device * device,InvokerContainer::Key key,bool down,float x,float y,float wheel)
	{
		if(!device)
			return 0;
		return device->onControl(key,down,x,y,wheel);
	}

	int onControl(InvokerContainer::Key key,bool down,float x,float y,float wheel)
	{
		int result=0;
		for(auto it=owner->devices.begin();it!=owner->devices.end();++it)
			result|=invoke(*it,key,down,x,y,wheel);	
		return result;
	}
	virtual void onEnable(bool flag){};
	virtual void render(HGE * hge);
	virtual void update(float dt);
	virtual bool add(Command *cmd);
	virtual void clear();
	Command * last();
	void cmdMoveTo(float x,float y,float distance);
	void cmdFollow(UnitPtr target);
	void cmdPatrol(float x,float y,float distance);
	void cmdAttack(GameObjectPtr unit);
	void cmdGuard(float x,float y,float distance);
	void cmdStand(float distance);
	/// from Mover::Driver::Listener
	void moverEvent(Mover *mover,int event);
	/// from Assembly::Listener
	void initDevices();

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);
};

class CmdAttack: public Command
{
public:	
	CmdAttack(GameObject * ptr);
	void moverEvent(Mover *mover,int event);
	virtual int process(Unit *unit);
	virtual void render(Unit * unit,HGE * hge);

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	virtual int getType() const {return cmdAttack;}
	GameObjectPtr target;
	enum
	{
		Pursue,
		Attacking,
	}attackState;

	CmdAttack();
};

class CmdMove: public Command
{
public:
	CmdMove(const Pose::pos &t,float minDist=25, bool patrol = false);

	virtual int process(Unit *unit);	
	void addPos(const Pose::pos &pos);
	virtual void render(Unit * unit,HGE * hge);
	/// from Mover::Driver::Listener
	virtual void moverEvent(Mover *mover,int event);

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	virtual int getType() const {return cmdMove;}
	CmdMove();
public:
	//Pose::pos target;
	//Pose::pos startPos;
	std::vector<Pose::pos> waypoints;
	int currentWaypoint;
	float minDist;
	bool onRoute;
	bool finished;
	bool patrol;
};

class CmdTakeItem: public Command
{
public:
	CmdTakeItem(Item *i,float dist);
	virtual bool check(Unit *unit);
	virtual int process(Unit *unit);
	void moverEvent(Mover *mover,int event);

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	virtual int getType() const {return cmdTakeItem;}
	CmdTakeItem();
public:
	bool moving;
	float minDist;
	Item *item;
};

class CmdDropItem: public Command
{
public:
	CmdDropItem(Item *i,const Pose::pos &targ);
	virtual bool check(Unit *unit);
	virtual int process(Unit *unit);

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	virtual int getType() const {return cmdDropItem;}
	CmdDropItem();
public:
	Item *item;
	Pose::pos target;
};

class CmdGuard : public Command
{
public:
	CmdGuard(const Pose::pos &targ,float maxDistance);
	virtual int process(Unit *unit);
	void moverEvent(Mover *mover,int event);

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	virtual int getType() const {return cmdGuard;}
	CmdGuard();
public:
	Pose::vec position;
	float maxDistance;
	enum CmdGuardState
	{
		cgsApproach,	/// moving to destination position. No pursuing
		cgsGuard,		/// waiting. 
		cgsPursue,
	}guardState;
};

//
//Command * orderMove(const Pose::pos &t,float minDist=25);
//
Controller * createCommandAI(Unit * u);
Controller * createPerceptionAI(Unit *u);