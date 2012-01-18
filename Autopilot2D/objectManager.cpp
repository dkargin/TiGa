#include "stdafx.h"
#include <functional>
#include "objectManager.h"
//#include "perception.h"
#include "unit.h"
#include "weapon.h"
#include <process.h>
/////////////////////////////////////////////////////////////
// globals
/////////////////////////////////////////////////////////////
extern Log * g_logger;

bool traceDead=false;
bool traceRemove=false;

struct NetCmdUseDevice
{
	ID unit;
	char device;
	char port;
	int action;
};
/////////////////////////////////////////////////////////////
// Client information
/////////////////////////////////////////////////////////////
ObjectManager::ClientInfo::ClientInfo():lastSync(-1),sendObjects(false),messages(NULL),updateObjects(false){}
ObjectManager::ClientInfo::~ClientInfo()
{
	cleanMessages();
}

bool ObjectManager::ClientInfo::haveMessages() const
{
	return messages!=NULL && messages->size()!=NULL;
}

IO::StreamOut & ObjectManager::ClientInfo::getMessages()
{
	if(!messages)
		messages = new IO::StreamOut(false);
	return *messages;
}
void ObjectManager::ClientInfo::cleanMessages()
{
	if(messages)
		delete messages;
	messages=NULL;
}
/////////////////////////////////////////////////////////////
// ObjectManager
/////////////////////////////////////////////////////////////
ObjectManager::ObjectManager(_Scripter *scripter, FxManager & fxManager)
:scripter(scripter)
,perceptionManager(NULL)
,pathCore(NULL)
,scene(NULL)
,role(Master)
,updatePeriod(10)
,clientsInfo(1)
,headerPos(-1)
,fxManager(fxManager)
,visionMode(VisionAll)
,pathFinder(NULL)
{
	LogFunction(*g_logger);
}

void ObjectManager::initSimulation( b2World * bs, pathProject::PathCore * pc )
{
	pathCore = pc;
	pathFinder.init(pc);
	scene = bs;
}

ObjectManager::~ObjectManager()
{
	LogFunction(*g_logger);
	//Objects::clear();	
	//Definitions::clear();
	/*
	if(perceptionManager)
	{
		delete perceptionManager;
		perceptionManager=NULL;
	}*/
}

void ObjectManager::initManagers()
{
	LogFunction(*g_logger);
	//perceptionManager=new PerceptionManager(this);
}

struct MsgCreateObject
{
	size_t id;
	size_t defid;
	unsigned short player;
	MsgCreateObject(size_t id, size_t defid, unsigned short player):id(id),defid(defid),player(player){}
};

void ObjectManager::saveState(IO::StreamOut & stream)
{
	cleanDead();
	//size_t objectsCount = objects.size();
	//stream.write(objectsCount);
	//auto end = objects.end();
	//for(auto it = objects.begin(); it != end; ++it)
	//{
	//	size_t start = stream.position();
	//	GameObject * object = *it;
	//	stream.write( MsgCreateObject(object, object->getDefinition()->id(), object->player) );
	//	size_t delta = stream.position() - start;
	//	object->save(stream);		
	//}
}

void ObjectManager::loadState(IO::StreamIn & stream)
{
	cleanDead();

	//size_t objectsCount = 0;
	//stream.read(objectsCount);

	//for(int i = 0; i < objectsCount; ++i)
	//{
	//	const MsgCreateObject * msg = stream.map<MsgCreateObject>();
	//	GameObject * object = create(msg->defid,msg->id,&stream);
	//	assert(object);
	//	object->load(stream);		
	//	object->update(0);
	//}
}

bool ObjectManager::canSee(const Pose & pose, float range, float fov, GameObject * object)
{
	Pose::dir sourceDir = pose.getDirection();
	Pose::vec delta = object->getPosition() - pose.getPosition(); 

	float distance = delta.length();

	if( distance < range )
		return false;

	float cangle = (sourceDir & delta)/( distance * sourceDir.length());
	return fov < 360.f ? cangle >= cos(fov * M_PI / 360.f) : true;
}

void ObjectManager::useDevice(Unit *unit, int device, int port, int action, IOBuffer *actionData)
{
	LogFunction(*g_logger);
	if(!unit)
		return;
	if(role==Master)	// if server - execute here
	{
		Device *d = unit->getDevice(device);
		if(d)
			d->execute(port,(DeviceCmd)action,actionData);
	}
	else				// if client - send data for remote execution
	{
		// TODO: reimplement
		/*
		std::for_each(clientsInfo.begin(),clientsInfo.end(),[&](ClientInfo & client)
		{
			MsgBuffer &buffer=client.getMessages();
			startHeader(buffer,unitUseDevice);
			NetCmdUseDevice cmd={unit->id(),device,port,action};
			buffer.write(cmd);			
			if(actionData)							
				buffer.write(*actionData);
			
			finishHeader(buffer);
		});		*/
	}
}

bool ObjectManager::controlObj(ID id,bool val)
{
	LogFunction(*g_logger);
	//if(!Objects::contains(id))
	//	return false;
	//Objects::get(id)->bLocal=val;
	return true;
}

GameObject *ObjectManager::create(ID defid, ID id , IO::StreamIn * context)
{
	LogFunction(*g_logger);
	//GameObjectDef *def=(GameObjectDef*)Definitions::objects[defid];
	//if(id==invalidID)
	//	return def->create(NULL);
	//else if(Objects::contains(id))
	//	return (GameObject*)Objects::objects[id];
	//else  
	//{
	//	GameObject * result = def->create(NULL);		
	//	Objects::remap(result->id(),id);
	//	//if(context)
	//	//	result->load(*context);
	//	return result;
	//}
	//g_logger->line(2,"ObjectManager::create(%d)-valid id for invalid object\n",id);
	return NULL;
}

GameObject * ObjectManager::create( GameObjectDef *def )	
{
	if(def)
		return def->create(NULL);
	g_logger->line(1,"Invalid definition");
	return NULL;
}

void ObjectManager::removeAllObjects()
{
	//Objects::clear();	
}

void ObjectManager::onFrameStart(float dt)
{
	if(role==Master)
		cleanDead();
	VisionManager::clear();
	// get paths
	pathFinder.harvest();
}
void ObjectManager::onFrameFinish(float dt)
{	
	for(Objects::iterator it=objects.begin();it!=objects.end();++it)
	{
		GameObject *u=(GameObject*)*it;
		u->update(dt);			
		if(u->isDead())		
			raiseObjectDead(u);
	}
//	perceptionManager->update(dt);
	fxManager.pyro.update(dt);
}
//////////////////////////////////////////////////////////////////////////
// Access methods
b2World* ObjectManager::getDynamics() 
{
	return scene;
}
FxManager* ObjectManager::getFxManager()
{
	return &fxManager;
}
lua_State* ObjectManager::getLua()
{
	return scripter->getVM();
}
_Scripter* ObjectManager::getScripter()
{
	return scripter;
}
//////////////////////////////////////////////////////////////////////////
// Graveyard manipulation
void ObjectManager::cleanDead()
{
	while(!deadList.empty())
	{
		delete *deadList.begin();
		deadList.erase(deadList.begin());
	}
}
///////////////////////////////////////////////////////////////////
/// listener manipulation
void ObjectManager::add(ObjectManager::Listener * listener)
{
	listeners.insert(listener);
}
void ObjectManager::remove(ObjectManager::Listener * listener)
{
	listeners.erase(listener);
}
void ObjectManager::raiseObjectDead(GameObject * object)
{
	// execute OnDie
	auto execOnDie = [object](Listener * listener)
	{
		if(listener)
			listener->onDie(object);
	};
	// call OnDie for all listeners
	std::for_each(listeners.begin(),listeners.end(),execOnDie);
	
	object->kill();
	deadList.push_back(object);
	if(traceDead)
		g_logger->line(0,"onDie...\n");
}

GameObject * ObjectManager::getObject(ObjID id)
{
	// TODO: implement
	return NULL;
}

void ObjectManager::onAdd(GameObject *object)
{	
}
void ObjectManager::onRemove(GameObject * object)
{
	LogFunction(*g_logger);
	ownedObjects.erase(object->id());
	// execute onDelete
	auto execOnRemove = [object](Listener * listener)
	{
		if(listener)
			listener->onDelete(object);
	};
	// call execution for each listener
	std::for_each(listeners.begin(),listeners.end(),execOnRemove);
	
	auto id = object->id();
	auto markDestroyed = [id](ClientInfo &info){info.destroyed.push_back(id);};
	// store id in destroyed container for each client
	std::for_each(clientsInfo.begin(),clientsInfo.end(),markDestroyed);
	// remove attached lua data. DODO: place it to LuaObject constructor, or replace it by Scripter::Object
	object->unbind(getLua());	
	if(traceRemove)
		g_logger->line(0,"onRemove...\n");
}
/////////////////////////////////////////////////////////////////
/// utilities
const Unit *ObjectManager::unit(const Objects::const_iterator &it)
{
	return dynamic_cast<Unit*>(*it);
}

Unit *ObjectManager::unit(const Objects::iterator &it)
{
	return dynamic_cast<Unit*>(*it);
}

long ObjectManager::getFrame() const
{
	return frame;
}

inline Unit * ObjectManager::getUnit(ObjID id)
{
	return dynamic_cast<Unit*>(getObject(id));
}

void ObjectManager::getObjects(const _Scripter::Object & table) const
{
	int i = 1;
	for(auto it = objects.begin();it != objects.end();++it, i++)
	{
		
		//lua_pushinteger(L,i);
		//LuaBox::TypeIO<GameObject*>::write(L,it->second);
		//lua_rawset(L,-3);		
	}
}

class Raycaster: public b2RayCastCallback
{	
public:	
	inline Raycaster(RayHits &hits,const Pose &ray):hits(hits),ray(ray){}	
	RayHits &hits;
	const Pose &ray;
	/*
	RayHits &cast(const Pose &r,float range)
	{
		ray=r;
		world->RayCast(this,b2conv(ray.position),b2conv(ray.position+range*ray.getDirection()));
		std::sort(hits.begin(),hits.end(),RayHitInfo::compare);
		return hits;
	}*/
	virtual float32 ReportFixture(	b2Fixture* fixture, const b2Vec2& point,const b2Vec2& normal, float32 fraction)
	{
		RayHitInfo hit;
		hit.point = conv(point);
		hit.normal = conv(normal);
		hit.object = (GameObject*)fixture->GetBody()->GetUserData();
		hit.fraction = fraction;
		hit.distance = vecDistance(ray.getPosition(),hit.point);
		hits.push_back(hit);
		return 1;
	}
};

void ObjectManager::raycast(const Pose &ray,float range,RayHits & hits)
{
	Raycaster raycaster(hits,ray);
	scene->RayCast(&raycaster,b2conv(ray.getPosition()),b2conv(ray.getPosition() + range * ray.getDirection()));
	std::sort(hits.begin(),hits.end(),RayHitInfo::compare);
}

GameObjectPtr ObjectManager::objectAtPoint(const Pose::pos & pos)
{
	auto objects = objectsAtRange(pos,0);
	return objects.empty()? NULL : objects.front();	
}

ObjectManager::ObjectList ObjectManager::objectsAtRange(const Pose::pos & pos,float range)
{
	ObjectList result;

	for(auto it = objects.begin();it!=objects.end();++it)
	{
		GameObject * object = *it;
		float distance = vecDistance(pos,object->getPosition());
		float size = object->getBoundingSphere().radius;
		if(distance < size + range)
			result.push_back(object);
	}

	return result;
}

ObjectManager::ObjectList ObjectManager::objectsAtSight(const Pose & pose, float range,float fov)
{
	ObjectList result;
	auto sourceDir=pose.getDirection();

	for(auto it = objects.begin();it!=objects.end();++it)
	{
		GameObject * object = *it;
		auto delta = object->getPosition()-pose.getPosition();  
		float distance = delta.length();
		float cangle = (sourceDir & delta)/(distance*sourceDir.length());
		if(fov < 360.f ? distance < range && cangle >= cos(fov * M_PI/360.f) : distance < range)
			result.push_back(object);
	}

	return result;
}

void ObjectManager::setVisionPlayer(int player)
{
	visionObject = NULL;
	visionPlayer = player;
	visionMode = VisionPlayer;
}

void ObjectManager::setVisionObject(GameObject * object)
{
	visionObject = object;
	visionPlayer = -1;
	visionMode = VisionObject;
}

void ObjectManager::setVisionAll()
{
	visionObject = NULL;
	visionPlayer = -1;
	visionMode = VisionAll;
}

bool ObjectManager::allowVision(GameObject * object)
{
	switch(visionMode)
	{
	case VisionObject:
		return object == visionObject;
	case VisionPlayer:
		return object->getPlayer() == visionPlayer;
	case VisionAll:
		return true;
	}
	return false;
}

static vec3f to3(const vec2f &v)
{
	return vec3(v[0],v[1],0);
}

const vec2f& to2(const vec3 &v)
{
	return *(const vec2f*)&v;
}

void PathFinder::Path::obtain(const pathProject::Waypoint * path, size_t count)
{
	decRef();
	if(count)
	{
		data = new Shared;
		data->refCounter = 1;
		
		
		std::vector<Waypoint2> tmp;
		Waypoint2 wp;
		tmp.reserve(count);
		bool skipActions = true;
		for(int i = count - 1; i >=0; i--)
		{
			if(!skipActions || i == count - 1 || i == 0 || path[i].action != path[i-1].action)
			{
				wp.v = vec2f(path[i].v[0], path[i].v[1]);
				wp.action = path[i].action;
				tmp.push_back(wp);
			}
		}

		data->data = new Path::Waypoint2[tmp.size()];
		for(size_t i = 0; i < tmp.size(); ++i)
		{
			data->data[i].v = tmp[i].v;
			data->data[i].action = tmp[i].action;
		}
		data->size = tmp.size();
	}
	else
	{
		data = NULL;
	}
}

struct PathFinderTask
{	
	pathProject::PathProcess * process;
	Pose source;	
	vec2f target;		
	PathFinder::Path path;
	PathFinder::TaskState state;
	PathFinder::Client * listener;
};

PathFinder::PathFinder(pathProject::PathCore * pathCore)
	:pathCore(pathCore), activeQuery(NULL),threadActive(false)
{
	thread = (HANDLE)_beginthreadex(NULL,0,(unsigned int(__stdcall *)(void*))PathFinder::PathfinderProcess,this,CREATE_SUSPENDED,0);
}

PathFinder::~PathFinder()
{
	TerminateThread(thread,0);

	for(auto it = queue.begin(); it!= queue.end(); ++it)
	{
		delete *it;
	}
	queue.clear();
	if(activeQuery)
	{
		delete activeQuery;
		activeQuery = NULL;
	}
}

PathFinder::TaskState PathFinder::taskState(PathFinderTask * task)
{
	Lockable::Scoped lock(activeQueryLock);
	return task->state;
}

void PathFinder::Client::cancelTask()
{
	if(task)
	{
		finder->cancel(task);
		task = NULL;
	}
}

void PathFinder::Client::createTask(pathProject::PathProcess * process, const Pose &source, const vec2f &target)
{
	cancelTask();	
	PathFinderTask * result = new PathFinderTask;
	if(result)
	{
		result->process = process;
		result->source = source;
		result->target = target;
		result->state = Idle;
		result->listener = this;
		
		task = result;

		this->finder->push(result);
	}
	else
		throw(std::exception("PathFinder::query(): allocation error"));
}

void PathFinder::cancel(PathFinderTask * task)
{
	Lockable::Scoped lock(activeQueryLock);
	task->listener = NULL;
}

void PathFinder::push(PathFinderTask * task)
{
	Lockable::Scoped lock(activeQueryLock);
	queue.push_back(task);
}

void PathFinder::checkTask()
{
	if(!threadActive && !queue.empty())
	{
		ResumeThread(thread);
	}
}

void PathFinder::harvest()
{
	activeQueryLock.lock();	
	while(!finished.empty())
	{
		PathFinderTask * task = finished.front();
		activeQueryLock.unlock();

		// call listener 
		if(task->listener)
			task->listener->getPath(task->path, task->state == Success);
		// reset listener link
		task->listener->task = NULL;
		task->listener = NULL;

		delete task;

		activeQueryLock.lock();
		finished.pop_front();
	}
	activeQueryLock.unlock();

	checkTask();
}


void PathFinder::runSearch()
{
	assert( pathCore != NULL );
	while(true)
	{
		threadActive = true;

		activeQueryLock.lock();
		if(!queue.empty())
		{
			activeQuery = queue.front();
			activeQuery->state = Run;
			queue.pop_front();
		}
		activeQueryLock.unlock();
		
		if(activeQuery)
		{
			activeQuery->process->addStart(to3(activeQuery->source.getPosition()),to3(activeQuery->source.getDirection()));
			activeQuery->process->addTarget(to3(activeQuery->target));

			pathCore->selectProcess(activeQuery->process);
			if(pathCore->doSearch_AStar())
			{
				activeQuery->state = Success;
				activeQuery->path.obtain(activeQuery->process->getPath(), activeQuery->process->getWaypointsCount());
			}
			else
			{
				activeQuery->state = Failed;
			}
			// move task to "finished"
			activeQueryLock.lock();
			finished.push_back(activeQuery);
			activeQuery = NULL;
			activeQueryLock.unlock();
		}
		else
		{
			threadActive = false;
			SuspendThread(GetCurrentThread());
		}
	}
}

unsigned int PathFinder::PathfinderProcess(void * arg)
{
	PathFinder * pathfinder = (PathFinder*)arg;
	pathfinder->runSearch();
	return 0;
}