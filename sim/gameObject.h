#pragma once

#include <vector>
#include <memory>

#include <fxobjects.h>

#include "solidobject.h"


namespace sim
{

// Header for network message
// TODO: Move it to a better place
struct MsgHeader
{
	int frame;		// message frame
	//int id;			// object ID
	int type;		// message type
	int length;		// message length
	MsgHeader():frame(0),type(0),length(0){}
	MsgHeader(int t,int f,int l):type(t),frame(f),length(l){}
};

enum ObjectType
{
	typeDummy,
	typeUnit,
	typeProjectile,
	typeItem,
	projectileDirect,
	projectileBallistic,
	projectileRay,
	projectileMissile,
	typeMover,
	typeBuilding,
	typeBlock,
	typeTree,
	typeVoid,
	typeMax
};

enum CollisionGroup
{
	cgStaticLow,
	cgStaticHigh,
	cgFlying,
	cgBody,				// custom body
	cgUnit,
	cgProjectile,
	cgShield,
	cgGhost,
	cgBorder,
	cgMax,
};


class PerceptionClient;

// Interface to any object to be synchronised through network
class NetObject
{
	friend class ObjectManager;
	bool bSync;			/// need to be synchronized
	bool bLocal;		/// if object is local
public:
	NetObject():bSync(false),bLocal(false){}
	// should this object be synchronised right now?
	virtual bool toSync()const
	{
		return bSync;
	}
	// mark this object for synchronisation
	virtual void toSync(bool flag)
	{
		bSync = flag;
	}
	// if object instance is updated locally on this server
	virtual bool local()
	{
		return bLocal;
	}	
	// update routines
#ifdef FUCK_THIS
	virtual int writeState(IO::StreamOut &buffer)=0;
	virtual int readState(IO::StreamIn &buffer)=0;
#endif
};

template<class Type> struct TypeInfo{};

class Unit;
class ObjectManager;
class GameObject;

#ifdef FUCK_THIS
// Basic definition for game object. 
class GameObjectDef: virtual public LuaObject, virtual public Referenced
{
public:
	typedef GameObject RootObject;
	typedef GameObjectDef RootObjectDef;
	typedef std::map<size_t, GameObjectDef * > Container;	
	friend class GameObject;
	friend class ObjectManager;

	size_t localID;						/// object id
	Container::iterator back;	/// back link for fast remove
	std::string name;					/// do we need this name?
	Fx::EntityPtr fxIdle;
	Fx::EntityPtr fxDie;
	BodyDesc body;

	GameObjectDef(ObjectManager *store);
	ObjectManager * manager;

	virtual ~GameObjectDef();
	virtual GameObject * create(IO::StreamIn *context)=0;
	virtual int init(){return 0;};
	_Scripter * getScripter();
	virtual GameObjectDef * clone()const =0;
	size_t getPopulation()const;	/// get current population
	size_t id() const {return localID;}	
protected:
	GameObjectDef(const GameObjectDef &def);
	void addRef();				/// increase population
	void decRef();				/// decrease population
private:
	size_t population;		/// current population	
};
#endif

typedef float Health;

class GameObject:
		public SolidObject,
		public NetObject,
		public std::enable_shared_from_this<GameObject>,
		public LuaBox::LuaObject
{
public:
	typedef GameObject RootObject;
	friend class ObjectManager;

	struct IDamageListener
	{
		virtual void onDamage(const Damage &dmg)=0;
	};

	// Moved from Desc
	std::string name;					/// do we need this name?
	Fx::EntityPtr fxIdle;
	Fx::EntityPtr fxDie;
	BodyDesc body;

	Fx::EntityPtr effects;
	IDamageListener * onDamage;
	int player;				///< the player owning this object
protected:	
	Health health, maxHealth;				///< current health
	CollisionGroup collisionGroup;	///< current collison group
	ObjectManager * manager;				///< where is it stored

public:
	GameObject(ObjectManager* parent);
	virtual ~GameObject();

	virtual void save(StreamOut & stream);
	virtual void load(StreamIn & stream);

	virtual GameObject* getDefinition() = 0;
	virtual ObjectManager* getManager();
	virtual ObjectType getType() const;
	virtual Scripter * getScripter();

	bool isUnique()const;

	float getSphereSize() const
	{
		return getBoundingSphere().radius;
	}

	virtual Sphere2 getBoundingSphere() const;
	virtual AABB2	getOOBB() const;			// get object oriented bounding box
	
	virtual void setCollisionGroup(CollisionGroup group);
	virtual CollisionGroup getCollisionGroup()const;

	virtual PerceptionClient * getPerceptionClient() { return NULL;};

	virtual void damage(const Damage &dmg);

	virtual bool isDead()const;

	virtual void kill();

	virtual Health getHealth() const;

	virtual int getPlayer() const;

	virtual void setPlayer(int p);

	virtual void update(float dt);  
	virtual int writeState(StreamOut &buffer);
	virtual int readState(StreamIn &buffer);
	size_t id() const{return localID;}
protected:
	size_t localID;			/// object id

private:
	GameObject* prototype;
};

typedef std::shared_ptr<GameObject> GameObjectPtr;

float GetCollisionSize(GameObject * object);

// completely remove object
void DestroyObject(GameObject * object);
// is object a hostile to object b
bool isHostile(const GameObject *a,const GameObject *b);

#define TO_STRING(value) (#value)
// installs some common methods related to definition in object-definition pair
#define OBJECT_DEF(Type,enumType) \
	ObjectType getType()const { return enumType;} \
	const char * luaName()const { return TO_STRING(Type##Def);} \
	Type##Def * copy()const { return new Type##Def(*this);} \
	RootObjectDef * clone()const { return copy();}

// installs some common methods related to object in object-definition pair
#define OBJECT_IMPL(Type,enumType) \
	ObjectType getType()const { return enumType;} \
	const char * luaName()const { return #Type;} \
	inline Type##Def& localDef(){return (Type##Def &)*definition;}

b2Body * createSolidBox(ObjectManager *m, float width, float height, float mass);
b2Body * createSolidSphere(ObjectManager *m, float size, float mass);

//void createObjectBox(GameObject * object, float width,float height,float mass);
//void createObjectSphere(GameObject *object, float size,float mass);

} // namespace sim
