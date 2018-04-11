#pragma once

#include <vector>
#include <memory>

#include "fx/fxobjects.h"

#include "forwards.h"
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
	virtual ~NetObject() {}
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
	virtual int writeState(StreamOut& buffer)=0;
	virtual int readState(StreamIn& buffer)=0;
};

template<class Type> struct TypeInfo{};

class ObjectManager;
class GameObject;


/**
 * Basic game object, that can be attached to simulation scene
 * Mixes functionality of
 *  - scripted object
 *  - wraps network state sync
 *  - interacts with scene
 */
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

	// Container for attached effects. We do not clone this generally
	// Should we use pointer, or can keep it on stack?
	Fx::Entity fx_root;
	IDamageListener* onDamage;
	int player;											//< the player owning this object

protected:
	// TODO: Move it to appropriate descendant
	Health health, maxHealth;				//< current health

	CollisionGroup collisionGroup;	//< current collison group
	ObjectManager * scene;					//< where is it stored

public:
	GameObject(GameObject* base);
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

	Fx::EntityPtr getGraphics();
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

	// Iterator in scene container.
	// Keep it for fast removal from the scene
	std::list<GameObjectPtr>::iterator scene_it;
};

float GetCollisionSize(GameObject * object);

// is object a hostile to object b
// This function is exposed to LUA
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

} // namespace sim
