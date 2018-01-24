#include "../sim/device.h"

#include "../sim/objectManager.h"
#include "../sim/unit.h"
#include "stdafx.h"

extern Log * g_logger;
//////////////////////////////////////////////////////////////////////
//
//DeviceManager::DeviceManager(ObjectManager *manager)
//	:objectManager(manager),luaState(manager->getLua())
//{}
//DeviceManager::~DeviceManager()
//{
//	clear();
//}
//////////////////////////////////////////////////////////////////////////
class CmdActionInvoker: public CmdInvoker
{
public:
	int keyAction;
	int invoke(Device *device,InvokerContainer::Key key,bool down,const Pose::pos &pos,float wheel)
	{
		if(key!=keyAction)
			return 0;
		device->query_Action(port);
		return 1;
	}
};

class CmdToggleInvoker: public CmdInvoker
{
public:
	int keyToggle;
	int invoke(Device *device,InvokerContainer::Key key,bool down,const Pose::pos &pos,float wheel)
	{		
		if(key != keyToggle)
			return 0;
		device->query_Toggle(port,down ? dcmdToggle_on:dcmdToggle_off);		
		return 1;
	}
};

class CmdParamInvoker: public CmdInvoker
{
public:
	int keyLeft,keyRight;
	int invoke(Device *device,InvokerContainer::Key key,bool down,const Pose::pos &pos,float wheel)
	{		
		if(key==keyLeft && down)
			device->query_Direction(port,dcmdDir_inc, 1.0f);
		if(key==keyLeft && !down)
			device->query_Direction(port,dcmdDir_inc,-1.0f);
		else if(key==keyRight)
			device->query_Direction(port,dcmdDir_inc,down?-1.0f: 1.0f);
		else
			return 0;
		return 1;
	}
};

class CmdTargetInvoker: public CmdInvoker
{
public:
	int keySet,keyReset,keyUpdate;
	int invoke(Device *device,InvokerContainer::Key key,bool down,const Pose::pos &pos,float wheel)
	{
		if(key==keySet && down)
			device->query_Target(port,dcmdTarget_set,pos);
		else if(key==keySet && !down)
			device->query_Target(port,dcmdTarget_reset,pos);
		else if(key==keyReset)
			device->query_Target(port,dcmdTarget_reset,pos);
		else if(key==keyUpdate)
			device->query_Target(port,dcmdTarget_update,pos);
		else
			return 0;
		return 1;
	}
};

void InvokerContainer::clean()
{
	for(auto it=commands.begin();it!=commands.end();++it)
		delete *it;
}

int InvokerContainer::invokerToggle(int port,InvokerContainer::Key toggle)
{
	CmdToggleInvoker *invoker=new CmdToggleInvoker;
	invoker->port = port;
	invoker->keyToggle = toggle;
	commands.push_back(invoker);
	return commands.size()-1;
}

int InvokerContainer::invokerParam(int port,InvokerContainer::Key kLeft,InvokerContainer::Key kRight)
{
	CmdParamInvoker *invoker=new CmdParamInvoker;
	//invoker->device=mount;
	invoker->port=port;
	invoker->keyLeft=kLeft;
	invoker->keyRight=kRight;
	commands.push_back(invoker);
	return commands.size()-1;
}

int InvokerContainer::invokerTarget(int port,InvokerContainer::Key kSet,InvokerContainer::Key kUpdate,InvokerContainer::Key kReset)
{
	CmdTargetInvoker *invoker=new CmdTargetInvoker;
	invoker->port = port;
	invoker->keySet=kSet;
	invoker->keyReset=kReset;
	invoker->keyUpdate=kUpdate;
	commands.push_back(invoker);
	return commands.size()-1;
}

int InvokerContainer::invokerAction(int port,InvokerContainer::Key action)
{
	CmdActionInvoker *invoker=new CmdActionInvoker;
	invoker->port=port;
	invoker->keyAction=action;
	commands.push_back(invoker);
	return commands.size()-1;
}

int InvokerContainer::onControl(Device * device,InvokerContainer::Key key,bool down,float x,float y,float wheel)
{
	if ( !useInvokers())
		return 0;
	for(auto it=commands.begin();it!=commands.end();++it)
	{
		CmdInvoker *invoker=*it;
		//Device *device=unit->getDevice(invoker->device);
		invoker->invoke(device,key,down,vec2f(x,y),wheel);
	}
	return 1;
}

bool InvokerContainer::useInvokers()const
{
	return true;
}
///////////////////////////////////////////////////////////////////////////
_Scripter * DeviceDef::getScripter()
{
	return manager.scripter;
}
///////////////////////////////////////////////////////////////////////////
// Device
///////////////////////////////////////////////////////////////////////////
Device::Device(DeviceDef *def)
:master(NULL),deviceMode(dmManual),definition(def), effects(def->manager.fxManager.get())
{	
	g_logger->line(0,"Device::Device");
	if(def->fxIdle)
	{
		fxIdle=def->fxIdle->clone();
		effects.attach(fxIdle);
	}
}

Device::~Device()
{
	g_logger->line(0,"Device::~Device");
}

DeviceDef * Device::getDefinition()
{
	return definition;
}

void Device::onInstall(Unit * master, size_t id, const Pose & pose)
{
	this->master = master;
	this->id = id;
	this->pose = pose;
}
//
//Device::Interfaces Device::getInterfaces()
//{
//	return Interfaces();
//}
int Device::onControl(InvokerContainer::Key key,bool down,float x,float y,float wheel)
{
	return getDefinition()->onControl(this,key,down,x,y,wheel);
}

Pose Device::getGlobalPose() const
{
	return master ? master->getPose() * pose : pose;
}

const char * Device::portDescription(int port)const
{
	return "generic port";
}

bool Device::validCommand(int port,DeviceCmd cmd)const
{
	return false;
}

int Device::query(int port,DeviceCmd action,IOBuffer *buffer)
{
	if(!master)
	{
		throw(std::exception("Device::Query - error: no master\n"));
		return 0;
	}
	if(!validCommand(port,action))
		return xWrongCmd();
	g_logger->line(0,"Device::query(%d,%d)\n", id, action);
	master->useDevice(id,port,action,buffer);
	
	return dcmdOk;
}

int Device::query_Action(int port)
{
	return query(port,dcmdAction);
}

int Device::query_Toggle(int port,DeviceCmd cmd)
{
	assert(cmdIsToggle(cmd));
	return query(port,cmd);
}

int Device::query_Direction(int port,DeviceCmd cmd,float delta)
{
	assert(cmdIsDirection(cmd));
	IOBuffer buffer(false,16);
	buffer.write(delta);
	buffer.flip();
	return query(port,cmd,&buffer);
}

int Device::query_Target(int port,DeviceCmd subtype,const Pose::pos &target)
{
	IOBuffer buffer(false,16);
	buffer.write(target);
	buffer.flip();
	return query(port,subtype,&buffer);
}

int Device::execute(int port,DeviceCmd action,IOBuffer *buffer)
{
	if(!validCommand(port,action))
		return xWrongCmd();
	
	if(cmdIsAction(action))
		execute_Action(port);
	else if(cmdIsToggle(action))
		execute_Toggle(port,action);
	else if(cmdIsDirection(action))
	{
		assert(buffer);
		float value;
		buffer->read(value);
		return execute_Direction(port,action,value);
	}
	else if(cmdIsTarget(action))
	{
		assert(buffer);
		Pose::pos target;
		buffer->read(target);
		return execute_Target(port, action,target);
	}
	else
		assert(false && "wrong action type");
	return dcmdWrongType;
}

int Device::execute_Action(int port)
{
	return dcmdNoImpl;
}

int Device::execute_Toggle(int port,DeviceCmd cmd)
{
	assert(cmdIsToggle(cmd));
	return dcmdNoImpl;
}

int Device::execute_Direction(int port,DeviceCmd cmd,float delta)
{
	assert(cmdIsDirection(cmd));
	return dcmdNoImpl;
}

int Device::execute_Target(int port,DeviceCmd cmd,const Pose::pos &target)
{
	assert(cmdIsTarget(cmd));
	return dcmdNoImpl;
}

int Device::getPorts()const
{
	return 0;
}

int Device::xWrongCmd()
{
	throw(std::exception("Device::xWrongCmd\n"));
	return dcmdWrongType;
}

int Device::xWrongPort()
{
	throw(std::exception("Device::xWrongPort\n"));
	return 0;
}

DeviceManager * Device::getManager()
{
	return &getDefinition()->manager;
}

b2Body * Device::getBody()
{
	if(dynamic_cast<Unit*>(master))
		return dynamic_cast<Unit*>(master)->getBody();
	return NULL;
}

const b2Body * Device::getBody()const
{
	if(dynamic_cast<const Unit*>(master))
		return dynamic_cast<const Unit*>(master)->getBody();
	return NULL;
}

void Device::setManual(bool flag)
{
	deviceMode = dmManual;
}
////////////////////////////////////////////////////////////////////////////////
// b2Body operation
const Pose::pos& Device::getPosition() const
{
	return getPose().getPosition();
}

Pose::vec Device::getDirection() const
{
	return getPose().axisX();
}

const Pose& Device::getPose() const
{
	return pose;
}
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
bool Device::takeControl(Device::Pointer device)
{
	if(!canControl(device))
		return false;
	if(device->deviceMode == dmSlave)	/// cannot take control over slave
		return false;	
	
	device->deviceMode = dmSlave;
	device->controller = this;

	return true;
}

bool Device::canControl(Device* device) const
{	
	return false;
}

bool Device::executeControl(Device * device, float dt)
{
	return false;
}

void Device::update(float dt)
{
	if(controller)
		controller->executeControl(this,dt);
}

void Device::setMode(DeviceMode mode)
{
	deviceMode = mode;
}

bool Device::useInvokers() const
{
	return deviceMode == dmManual;
}