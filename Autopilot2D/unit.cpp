#include "stdafx.h"
//#include "world.h"
#include "objectManager.h"
#include "device.h"
#include "unit.h"
#include "mover.h"
#include "perception.h"
#include "commandAI.h"
#include "inventory.h"
#include "projectile.h"

//#include "bind.h"
#include "weapon.h"

////////////////////////////////////////////////////////
// UnitDef
////////////////////////////////////////////////////////
UnitDef::UnitDef(ObjectManager *m)
:GameObjectDef(m),fxMove(NULL)//,perception(NULL)
{
	LogFunction(*g_logger);
}

UnitDef::UnitDef(const UnitDef& def)
	:GameObjectDef(def)
{
	fxMove = def.fxMove;
	health = def.health;
}

UnitDef::~UnitDef()
{
	LogFunction(*g_logger);
	if(fxMove)delete fxMove;
}

int UnitDef::init()
{
	LogFunction(*g_logger);
	if(lua)
	{
		_Scripter * scripter = getScripter();
		_Scripter::Object object = scripter->getRegistry(lua);	// stack={def->lua}
		object.call("onInit");
	}
	else
		g_logger->line(2,"wrong luaRef");
	//callOnInit();
	return 1;
}

Unit * UnitDef::construct(IO::StreamIn *context)
{
	LogFunction(*g_logger);
	Unit * result = new Unit(this);
	if( result )
	{
		if(lua)
		{
			_Scripter * scripter = getScripter();
			_Scripter::Object object = scripter->getRegistry(lua);	// stack={def->lua}
			object.call("onCreate", (LuaObject*)result);
		}
		else
			g_logger->line(2,"wrong luaRef");
	
		if(!result->getBody())
			result->attachBody(createSolidSphere(manager,20,1.0));
	}
	else
		g_logger->line(2,"memory error: cannot create Unit");
	return result;
}

GameObject * UnitDef::create(IO::StreamIn * buffer)
{
	return construct(buffer);
}

size_t UnitDef::addMount(const MountDef& mount)
{
	//struct test{int a; test():a(10){}};
	//void * ptr = 0;
	//test * data = new(ptr) test;	
	mounts.push_back(mount);
	return mounts.size() - 1;
}

void UnitDef::clearMounts()
{
	mounts.clear();
}
////////////////////////////////////////////////////////
// Unit
////////////////////////////////////////////////////////
Unit::Unit(UnitDef *def)
:definition(def)
,useAI(true)
,GameObject(def->manager,def)
,controller(new Controller(this))
{	
	LogFunction(*g_logger);
	health=definition->health;	
	setCollisionGroup(cgUnit);
	if(def->fxIdle)
	{
		effects->attach(def->fxIdle->clone());	
		effects->start();
	}
	// install all the devices from the definition
	for_each(definition->mounts.begin(),definition->mounts.end(),[this](MountDef & mount)
	{
		Device * device = mount.device->create();
		if(device)
		{
			devices.push_back(device);			
			device->onInstall(this, devices.size()-1, mount.pose);
		}
	});

	controller->initDevices();
}

Unit::~Unit()
{
	LogFunction(*g_logger);
	if(controller)
	{
		delete controller;
		controller=NULL;
	}
	for(auto it = devices.begin(); it != devices.end(); ++it)
	{
		Device * device = *it;
		delete device;
	}
	//if(mover)
	//	delete mover;
	//if(perception)
	//	delete perception;
}

void Unit::useDevice(int device,int port,int action,IOBuffer *buffer)
{
	LogFunction(*g_logger);
	getManager()->useDevice(this,device,port,action,buffer);
}

Device * Unit::getDevice(size_t id)
{
	LogFunction(*g_logger);
	return devices[id];
}

PerceptionClient * Unit::getPerceptionClient()
{
	LogFunction(*g_logger);
	return controller;
}
/*
int Unit::readDesc(IOBuffer &buffer)
{
	//for(auto it=mounts.begin();it!=mounts.end();++it)
	//	it->device->writeBuildContext(context);
	return 0;
}

int Unit::writeDesc(IOBuffer &context)
{
	for(auto it=mounts.begin();it!=mounts.end();++it)
		it->device->writeDesc(context);
	return 1;
}
*/
Controller * Unit::getController()
{
	return controller;
}

void Unit::setController(Controller * cai)
{
	LogFunction(*g_logger);
	if(controller && controller != cai)
	{
		delete controller;
		controller=NULL;
	}
	controller = cai;
	useAI = true;
	controller->initDevices();
}

void Unit::enableAI(bool flag)
{
	LogFunction(*g_logger);
	if(useAI!=flag)
	{
		useAI=flag;
		if(controller)
			controller->onEnable(flag);
		for(auto it = devices.begin();it != devices.end();++it)
			if( *it)
				(*it)->setManual(!flag);
	}	
}

void Unit::update(float dt)
{
	LogFunction(*g_logger);
	if(useAI && controller)
		controller->update(dt);
	GameObject::update(dt);
	for(auto it = devices.begin();it != devices.end();++it)
		(*it)->update(dt);
}

void Unit::startAnimation(const char * name)
{
}

void Unit::stopAnimation(const char *name)
{
}

bool Unit::toSync() const
{
	bool res=toSync();
	for(auto it = devices.begin();it != devices.end();++it)
		res|=(*it)->toSync();
	return res;
}

void Unit::toSync(bool val)
{
	for(auto it = devices.begin();it != devices.end();++it)
		(*it)->toSync(val);
}

int Unit::writeState(IO::StreamOut &buffer)
{
	LogFunction(*g_logger);
	GameObject::writeState(buffer);
	for(auto it = devices.begin();it != devices.end();++it)
		(*it)->writeState(buffer);
	return 1;
}

int Unit::readState(IO::StreamIn &buffer)
{
	LogFunction(*g_logger);
	GameObject::readState(buffer);
	for(auto it = devices.begin();it != devices.end();++it)
		(*it)->readState(buffer);
	return 1;
}

void Unit::save(IO::StreamOut & stream)
{
	writeState(stream);
	controller->save(stream);
}

void Unit::load(IO::StreamIn & stream)
{
	readState(stream);
	controller->load(stream);
}
/////////////////////////////////////////////////////////////////////////////////////////
void Unit::cmdStop()
{
	if(controller)
		controller->clear();
}

void Unit::cmdStand(float distance)
{
	if(controller)
		controller->cmdStand(distance);
}

void Unit::cmdMoveTo(float x,float y,float distance)
{
	if(controller)
		controller->cmdMoveTo(x,y,distance);
}

void Unit::cmdFollow(Unit * target)
{
	if(controller)
		controller->cmdFollow(UnitPtr(target));
}

void Unit::cmdPatrol(float x,float y,float distance)
{
	if(controller)
		controller->cmdPatrol(x,y,distance);
}

void Unit::cmdAttack(GameObject* target)
{
	if(controller)
		controller->cmdAttack(target);
}

void Unit::cmdGuard(float x,float y,float distance)
{
	if(controller)
		controller->cmdGuard(x,y,distance);
}
///////////////////////////////////////////////////////////////////////////////////////
// sometimes LUA/tolua don't want to compare Unit* pointers directly. Use this function instead
bool equal(const GameObject * a,const GameObject * b)
{
	return a==b;
}
/////////////////////////////////////////////////////////////////
// to treat GameObject * as Unit * in lua code
Unit *toUnit(GameObject *object)
{
	return dynamic_cast<Unit*>(object);
}