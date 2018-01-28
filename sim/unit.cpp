#include "unit.h"
#include "commandAI.h"
#include "device.h"
#include "objectManager.h"
#include "mover.h"
#include "inventory.h"

#include "perception.h"
#include "projectile.h"
#include "weapon.h"

namespace sim
{
#ifdef FUCK_THIS
UnitDef::UnitDef(ObjectManager *m)
:GameObjectDef(m),fxMove(NULL)//,perception(NULL)
{
//	LogFunction(*g_logger);
}

UnitDef::UnitDef(const UnitDef& def)
	:GameObjectDef(def)
{
	fxMove = def.fxMove;
	health = def.health;
}

UnitDef::~UnitDef()
{
//	LogFunction(*g_logger);
	if(fxMove)delete fxMove;
}

int UnitDef::init()
{
//	LogFunction(*g_logger);
	if(lua)
	{
		_Scripter * scripter = getScripter();
		_Scripter::Object object = scripter->getRegistry(lua);	// stack={def->lua}
		object.call("onInit");
	}
//	else
//		g_logger->line(2,"wrong luaRef");
	//callOnInit();
	return 1;
}

Unit * UnitDef::construct(IO::StreamIn *context)
{
//	LogFunction(*g_logger);
	Unit * result = new Unit(this);
	if( result )
	{
		if(lua)
		{
			_Scripter * scripter = getScripter();
			_Scripter::Object object = scripter->getRegistry(lua);	// stack={def->lua}
			object.call("onCreate", (LuaObject*)result);
		}
//		else
//			g_logger->line(2,"wrong luaRef");
	
		if(!result->getBody())
			result->attachBody(createSolidSphere(manager,20,1.0));
	}
//	else
//		g_logger->line(2,"memory error: cannot create Unit");
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

#endif
////////////////////////////////////////////////////////
// Unit
////////////////////////////////////////////////////////
Unit::Unit(Unit* def)
:GameObject(def)
{
	useAI = false;
	local = true;

	if(def != nullptr)
	{
		if(def->fxIdle)
		{
			fxIdle = def->fxIdle->clone();
		}

		// install all the devices from the definition
		for(const MountDef& mount: def->mounts)
		{
			if(mount.device)
			{
				Device* device = mount.device->clone();
				devices.push_back(device);
				device->onInstall(this, devices.size()-1, mount.pose);
			}
		};

		health = def->health;
		definition = def;
	}

	if(fxIdle)
	{
		fx_root.attach(fxIdle);
		fx_root.start();
	}

	controller.reset(new Controller(this));

	setCollisionGroup(cgUnit);

	controller->initDevices();
}

Unit::~Unit()
{
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
//	LogFunction(*g_logger);
	getManager()->useDevice(this,device,port,action,buffer);
}

Device * Unit::getDevice(size_t id)
{
//	LogFunction(*g_logger);
	return devices[id];
}

PerceptionClient * Unit::getPerceptionClient()
{
//	LogFunction(*g_logger);
	return controller.get();
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
	return controller.get();
}

void Unit::setController(Controller * cai)
{
	useAI = true;
	controller.reset(cai);
	controller->initDevices();
}

void Unit::enableAI(bool flag)
{
//	LogFunction(*g_logger);
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
//	LogFunction(*g_logger);
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

b2World * Unit::getDynamics()
{
	return getManager()->scene;
}

int Unit::writeState(StreamOut &buffer)
{
//	LogFunction(*g_logger);
	GameObject::writeState(buffer);
	for(auto it = devices.begin();it != devices.end();++it)
		(*it)->writeState(buffer);
	return 1;
}

int Unit::readState(StreamIn &buffer)
{
//	LogFunction(*g_logger);
	GameObject::readState(buffer);
	for(auto it = devices.begin();it != devices.end();++it)
		(*it)->readState(buffer);
	return 1;
}

void Unit::save(StreamOut & stream)
{
	writeState(stream);
	controller->save(stream);
}

void Unit::load(StreamIn & stream)
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

void Unit::cmdAttack(GameObjectPtr target)
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

}
