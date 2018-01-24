#pragma once
#include "../sim/gameObject.h"
// generates ID for GameObject
typedef unsigned int ID;
const ID invalidID = -1;
typedef ID DefID;	// object definition identifier
typedef ID ObjID;	// object identifier

class IOBuffer;

enum DeviceCmd
{	
	dcmdAction,
	dcmdToggle_on,
	dcmdToggle_off,
	dcmdDir_inc,
	dcmdDir_set,
	dcmdTarget_set,
	dcmdTarget_reset,
	dcmdTarget_update,
/////////////////////////
	dcmdTotal
};

inline bool cmdIsAction(DeviceCmd cmd)
{
	return cmd == dcmdAction;
}

inline bool cmdIsToggle(DeviceCmd cmd)
{
	return cmd == dcmdToggle_on || cmd == dcmdToggle_off;
}

inline bool cmdIsDirection(DeviceCmd cmd)
{
	return cmd == dcmdDir_set || cmd == dcmdDir_inc;
}

inline bool cmdIsTarget(DeviceCmd cmd)
{
	return cmd == dcmdTarget_set || cmd == dcmdTarget_reset || cmd == dcmdTarget_update;
}


enum DeviceResult
{
	dcmdOk=1,
	dcmdSent,
	dcmdNoImpl,
	dcmdWrongType,
	dcmdWrongPort,
	dcmdNoMaster,
};
namespace NetCmd
{	
	enum Type
	{
		cmdInvalid,
		cmdConfirm,
		cmdShoot,
		cmdProjectileImpact,
		cmdTakeControl,
		cmdHello,
	};
	class Base
	{
	public:		
		virtual ~Base(){}
		virtual Type type()const=0;
		virtual bool confirm()const =0;				//  send confirmaton when recieve
		virtual bool remote()const=0;				//	remote command (send to all or execute only locally)
		virtual bool read(IO::StreamIn &stream)=0;
		virtual bool write(IO::StreamOut &stream)=0;
		virtual Base * toBase()=0;
	};
	template<bool bRemote,bool bConfirm,NetCmd::Type lType,class Target> class NetCmdHelper: public Base
	{
	public:
		static Base * create(){return new Target;}
		virtual NetCmd::Type type()const{return lType;}
		virtual bool confirm()const		{return bConfirm;}
		virtual bool remote()const		{return bRemote;}
		virtual Base * toBase()			{return this;}
		virtual bool read(IO::StreamIn &stream){return true;}
		virtual bool write(IO::StreamOut &stream){return true;}
	};
	// every peer send it on connection
	struct Hello: public NetCmdHelper<true,false,cmdHello,Hello>{};

	struct TakeControl: public NetCmdHelper<true,true,cmdTakeControl,TakeControl>
	{
		ObjID unit;
		TakeControl():unit(-1){};
		TakeControl(Unit *obj);;
		virtual bool read(IO::StreamIn &stream);
		virtual bool write(IO::StreamOut &stream);
	};	
	TakeControl *toTakeControl(Base *base);
	Hello *toHello(Base *base);
};
///////////////////////////////////////////////////////////////////////////////
struct RayHitInfo
{
	GameObject *object;
	vec2f point,normal;
	float fraction;
	float distance;
	static bool compare(const RayHitInfo &a,const RayHitInfo &b)
	{
		return a.distance<b.distance;
	}
};

typedef std::vector<RayHitInfo> RayHits;

class VisionManager
{
public:
	struct Vision
	{
		int player;
		float distance;
		float fov;
		Pose pose;
	};
	typedef std::vector<Vision> Container;
	enum {ReservedVision=256};
	Container vision;
	VisionManager()
	{
		vision.reserve(ReservedVision);
	}
	void clear()
	{
		vision.resize(0);
	}
	virtual bool allowVision(GameObject * owner)
	{
		return true;
	}
	virtual bool addVision(float distance,float fov,const Pose &pose, GameObject * owner)
	{
		if(owner && allowVision(owner))
		{
			Vision res={ owner->getPlayer(), distance, fov, pose };
			vision.push_back(res);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////////
// PathFinder in a separate thread
///////////////////////////////////////////////////////////////////
struct PathFinderTask;

class PathFinder
{
public:
	class Path;

	class Client
	{
		friend class PathFinder;
	public:		
		Client(PathFinder * finder):task(NULL),finder(finder){}
		virtual ~Client()
		{
			cancelTask();
		}
		virtual void getPath(Path & ptr, bool success){}
		void cancelTask();
		void createTask(pathProject::PathProcess * process, const Pose & source, const vec2f & to);
	private:
		PathFinderTask * task;
		PathFinder * finder;
	};

	//PathFinderTask * createTask(Listener * listener, pathProject::PathProcess * process, const Pose & source, const vec2f & to);

	void remove(PathFinderTask * query);

	enum TaskState
	{
		Idle,	// no task, no result
		Run,
		// waiting until result would be retrieved
		Success,	// path was found
		Failed,		// path was not found
	};

	TaskState taskState(PathFinderTask * query);

	bool taskReady(PathFinderTask * task);

	PathFinder(pathProject::PathCore * pathCore);
	~PathFinder();
	bool valid() const;
	void harvest();
	void init(pathProject::PathCore * pc ) 
	{ 
		assert( pathCore == NULL );
		pathCore = pc;
	}
protected:
	void push(PathFinderTask * task);
	// called by client to reject task
	void cancel(PathFinderTask * task);

	Lockable activeQueryLock;

	SharedValue<bool> threadActive;

	pathProject::PathCore * pathCore;
	// check active task
	void checkTask();

	typedef std::list<PathFinderTask*> Queue;
	Queue queue;	// task queue
	Queue finished;
	PathFinderTask * activeQuery;
	void runSearch();
	static unsigned int __stdcall PathfinderProcess(void * pathfinder);
	HANDLE thread;
};

class PathFinder::Path
{
public:
	Path():data(NULL){}

	explicit Path(const Path &path):data(path.data)
	{
		incRef();
	}
	virtual ~Path()
	{
		decRef();
	}

	struct Waypoint2
	{
		vec2f v;
		int action;
	};

	void obtain(const pathProject::Waypoint * path, size_t count);

	operator const Waypoint2 *() const
	{
		return valid() ? data->data : NULL;
	}

	size_t size() const
	{
		return valid() ? data->size : 0;
	}

	Path & operator = (const Path &path)
	{
		if(data != path.data)
		{
			decRef();
			data = path.data;
			incRef();
		}
		return *this;
	}
	bool operator == (const Path &path)
	{
		return data == path.data;
	}
	bool operator != (const Path &path)
	{
		return data != path.data;
	}
	bool valid() const
	{
		return data != NULL;
	}
protected:
	void incRef()
	{
		if(data)
		{
			data->refCounter++;
		}
	}
	void decRef()
	{
		if(data && !--data->refCounter)
		{
			// delete path
			delete[] data->data;
			data->data = NULL;
			// delete path header
			delete data;
			data = NULL;
		}		
	}
	// this part is shared between several Path instances
	struct Shared
	{
		Waypoint2 * data;	// pointer to path
		size_t size;		// path length
		size_t refCounter;	// reference counter
	}*data;
};

class Device;
class DeviceDef;

/// Iterator for gameobjects list
struct ObjectIterator
{
	ObjectManager * container;
	GameObject * current;
};
///////////////////////////////////////////////////////////////////////////////
// Manages creation, storage, serialisation and net synchronisation of game objects
///////////////////////////////////////////////////////////////////////////////
class ObjectManager :	public VisionManager
{	
public:		
	class Listener
	{
	public:
		virtual void onDie(GameObject *unit){};
		virtual void onDelete(GameObject *unit){};
	};

	enum NetMessages
	{
		objectCreate=0x1f,
		objectDelete,
		objectDead,
		objectUpdate,
		unitUseDevice,
	};

	friend class PerceptionManager;
	friend class WeponManager;

	typedef ObjStore<GameObjectDef> Definitions;

	typedef std::list< GameObject* > ObjectList; 
	std::list< DeviceDef * > deviceDefinitions;
	std::list< GameObjectDef * > objectDefinitions;

	GameObject * objectsHead, * objectsTail;
	// theese are for unit parts
	PerceptionManager *perceptionManager;	// replace it by DeviceManager in future	
	FxManager::SharedPtr fxManager;
	//b2BlockAllocator allocator;
	b2World * scene;
	pathProject::PathCore * pathCore;
	PathFinder pathFinder;
	
	enum Role
	{
		Master,
		Slave,
		Assistant,
	}role;
	Scripter *scripter;	
public:
	ObjectManager(Scripter *lua, FxManager::SharedPtr fx);	
	virtual ~ObjectManager();

	void initSimulation( b2World * dn, pathProject::PathCore *pathCore );

	virtual void saveState(IO::StreamOut & stream);
	virtual void loadState(IO::StreamIn & stream);

	void registerObject(GameObject * object);
	void removeObject(GameObject * object);
	void removeAllObjects();/// remove all objects from the map

	virtual void initManagers();

	void onFrameStart(float dt);
	void onFrameFinish(float dt);
	long getFrame()const;

	void addListener(ObjectManager::Listener * listener);
	void removeListener(ObjectManager::Listener * listener);

	GameObject * getObject(ObjID id);
	Unit * getUnit(ObjID id);
	/// spatial queries
	// raycasting, returns sorted hit list
	void raycast(const Pose &ray,float range,RayHits & hits);
	// generic raycast
	template<typename Callback> void raycastGeneric(const Pose & ray, float maxRange, Callback & callback)
	{
		class Local: public b2RayCastCallback
		{	
		public:	
			Callback & callback;
			const Pose & ray;
			Local(const Pose & ray, Callback & callback):ray(ray),callback(callback){}
			virtual float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction)
			{
				RayHitInfo hit;
				hit.point=conv(point);
				hit.normal=conv(normal);
				hit.object=(GameObject*)fixture->GetBody()->GetUserData();
				hit.fraction=fraction;
				hit.distance=vecDistance(ray.position,hit.point);
				callback(hit);
				return 1;
			}
		}raycaster(ray, callback);
		scene->RayCast(&raycaster,b2conv(ray.position),b2conv(ray.position + maxRange * ray.getDirection()));
	}

	GameObjectPtr objectAtPoint(const Pose::pos & pos);							/// get object in specific position
	ObjectList objectsAtRange(const Pose::pos & pos,float distance);			/// get objects in circle area
	ObjectList objectsAtSight(const Pose & pose, float distance,float fov);		/// get objects in vision segment

	//virtual void updatePerception(Perception * p,float dt);
	virtual bool canSee(const Pose & pose, float distance, float fov, GameObject * object);
	/// networks
	//void readMessages(IO::StreamIn &buf,int client=0);
	//void writeMessages(IO::StreamOut &buf,int client=0);	
	void useDevice(Unit *unit,int device,int port,int action,IOBuffer *buffer);	

	// vision control
	enum VisionMode
	{
		VisionAll,
		VisionObject,
		VisionPlayer,
	}visionMode;

	GameObjectPtr visionObject;
	int visionPlayer;

	void setVisionAll();
	void setVisionPlayer(int player);
	void setVisionObject(GameObject * object);

	virtual bool allowVision(GameObject * object);
	
	/// access methods
	b2World* getDynamics();
	FxManager* getFxManager();
	lua_State* getLua();
	_Scripter* getScripter();
protected:

	struct ClientInfo
	{
		bool sendObjects;			//< send objectCreate msg for all objects
		bool updateObjects;		//< send objectUpdate msg for all objects
		long lastSync;
		std::set<ID> objects;
		std::list<ID> destroyed;
		IO::StreamOut *messages;		// additional data to send
		ClientInfo();
		~ClientInfo();
		bool haveMessages()const;
		IO::StreamOut & getMessages();
		void cleanMessages();
	};

	int updatePeriod,frame;
	typedef std::vector<ClientInfo> ClientsInfo;
	std::set<ID> ownedObjects;
	ClientsInfo clientsInfo;
	ID newObjID;

	// local object creation
	//GameObject * create(ID defid, ID id, IO::StreamIn * context);
	// remote object creation
	//bool ownDef(ID id);
	void cleanDead();			// destroy all objects on the graveyard
	void raiseObjectDead(GameObject * unit);	// send unit to the graveyard and raise "onDie" event

	void onAdd(GameObject *object);
	void onRemove(GameObject *object);
	
	// here all dead units are stored to the end of frame. All contained objects are removed on the next frame
	typedef std::list<GameObject*> DeadList;
	DeadList deadList;
	typedef std::set<Listener*> Listeners;
	Listeners listeners;

	//void writeObjectUpdate(GameObject *object,MsgBuffer &buf);

	// Get this shit out of here
	void startHeader(IO::StreamOut &buffer,int type);
	int finishHeader(IO::StreamOut &buffer);
	int headerPos;
};

int randRange(int min,int max);
