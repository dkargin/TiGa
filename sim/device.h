#pragma once

#include "basetypes.h"
#include "objectManager.h"

namespace sim
{

class Assembly;
class Device;

typedef int DCmd;
const int DeviceMaxPorts = 4;

template<class Type,int N> inline std::vector<Type> makeVec(Type source[N])
{
	return std::vector<Type>(source,source+N);
}
///////////////////////////////////////////////////////////////////////////
/// base class for any invoker - helper that converts keyboard 
/// and mouse input to specific device command.
/// Every device which can be used in manual (controlled by keyboard&mouse) 
/// mode, has its own set of invokers
///////////////////////////////////////////////////////////////////////////
class CmdInvoker
{
public:
	typedef int Key;
	//int device;		//	device index
	int port;		//	command port
	virtual ~CmdInvoker(){};
	virtual int invoke(Device *device, Key key, int down, const Pose::pos &pos, float wheel) =0;
};
///////////////////////////////////////////////////////////////////////////
/// Contains the set of invokers
///////////////////////////////////////////////////////////////////////////
class InvokerContainer
{
public:
	typedef int Key;
	typedef int DeviceIndex;	
	std::list<CmdInvoker*> commands;
	
	virtual ~InvokerContainer(){clean();}
	// clean all invokers
	void clean();
	/// invoker gets control (keyboard or mouse) command and sends it directly to device
	// add invoker for dcmdDir
	int invokerParam(int port,Key kLeft,Key kRight);
	//
	int invokerToggle(int port,Key toggle);
	// add invoker for dcmdTarget
	int invokerTarget(int port,Key kSet,Key kUpdate,Key kClear);
	// add invoker for dcmdAction
	int invokerAction(int port,Key action);
	// invoke actions
	int onControl(Device * device, Key key, int status, float x, float y, float wheel);
	/// if we use invoker system
	virtual bool useInvokers() const;
};

enum DeviceMode
{
	dmOffline,			/// device is turned off
	dmDriver,			/// device is controlled by its driver
	dmSlave,			/// device is controlled by other device or network
	dmManual,			/// device is controlled by player manually (keyboard + mouse?)
};

/// Description for mounting point
/// Device can be attached to mount points
struct MountDef
{	
	Pose pose;
	Device* device = nullptr;
};
/////////////////////////////////////////////////////////////////////////
// Base class for any device that can be mounted on unit
/////////////////////////////////////////////////////////////////////////
class Device:
		public NetObject,
		public std::enable_shared_from_this<Device>
{
	friend ObjectManager;
	friend bool connectDevices(Device * controller, Device *controlled);
public:
	typedef Device RootObject;
	typedef std::shared_ptr<Device> Pointer;
	typedef ObjectManager Manager;

	Device(Device* prototype=nullptr);
	virtual ~Device();

	virtual void onInstall(Unit * unit, size_t id, const Pose & pose);	// called when device installed into an assembly
	
	virtual void update(float dt);

#ifdef DEVICE_RENDER
	virtual void render(HGE * hge){}
#endif
	virtual ObjectManager * getManager();
	virtual Device * getDefinition();
	virtual b2Body * getBody();
	virtual Unit * getMaster(){return master;}
	virtual const b2Body * getBody()const;

	/// control interfaces
	virtual bool canControl(const Device * device) const;
	virtual bool takeControl(Device::Pointer device);
	/// ports
	virtual int getPorts()const;									/// return active control ports
	virtual bool validCommand(int port,DeviceCmd cmd)const=0;		/// is valid command type for port
	virtual const char * portDescription(int port)const;
	/// query action execution
	virtual int query(int port,DeviceCmd action,IOBuffer *buffer=NULL);					/// general query
	virtual int query_Action(int port);												/// query action command
	virtual int query_Toggle(int port,DeviceCmd subtype);							/// query toggle command
	virtual int query_Direction(int port,DeviceCmd subtype,float delta);			/// query direction(value) command
	virtual int query_Target(int port,DeviceCmd subtype,const Pose::pos &target);	/// query target command
	/// action execution
	virtual int execute_Action(int port);
	virtual int execute_Toggle(int port,DeviceCmd subtype);
	virtual int execute_Direction(int port,DeviceCmd subtype,float delta);
	virtual int execute_Target(int port,DeviceCmd subtype,const Pose::pos &target);
	virtual int execute(int port,DeviceCmd action,IOBuffer *buffer = NULL);	/// command execution code for server
	
	// invoke actions
	int onControl(InvokerContainer::Key key, int down,float x,float y,float wheel);
	// use invokers only in manual mode
	bool useInvokers()const;
	//// process controls. Called from LUA
	void setManual(bool flag);	/// disable driver control
	
	// pose operation
	virtual const Pose::pos& getPosition() const;
	virtual Pose::vec getDirection() const;
	virtual const Pose& getPose()const;
	virtual Pose getGlobalPose() const;
protected:
	virtual bool executeControl(Device * device, float dt);

public:
	Fx::EntityPtr effectContainer;
	Fx::EntityPtr fxIdle;
	int lua;
	DeviceMode deviceMode;
protected:
	Pointer parent;
	Pointer controller;		///< controlling device
	Unit * master;				///< device owner
	int id;								///< device index in unit's array
	Pose pose;						///< relative pose

	void setMode(DeviceMode mode);
	int xWrongPort();
	int xWrongCmd();
private:
	Device* prototype;
};

typedef ObjectManager DeviceManager;
#ifdef FUCK_THIS
class DeviceDef: public LuaObject, public InvokerContainer
{
protected:
	friend class ObjectManager;	
public:	
	typedef Device RootObject;
	typedef DeviceDef RootObjectDef;

	std::string name;					// object name

	Fx::EntityPtr fxIdle;			// effects for idle state

	DeviceDef(ObjectManager & manager):manager(manager){}
	DeviceDef(const DeviceDef &def):manager(def.manager){}
	virtual ~DeviceDef(){}

	virtual Device * create(StreamIn *context = NULL) = 0;
	int init();	

	Scripter * getScripter();

	ObjectManager &manager;	
};

#endif

}
