#ifndef GAME_OBJECT
#define GAME_OBJECT

//#include "ioTools.h"

#pragma once
#pragma warning(disable:4250)
extern Log * g_logger;

class GameWorld;
class _Scripter;
class b2Body;
struct lua_State;
template<class T> class ObjStore;

// Header for network message
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
typedef Pose2z Pose;
class PoseIn
{
public:
	virtual const Pose::pos2 & getPosition() const =0;
	virtual Pose::vec2 getDirection() const=0;
	virtual Pose getPose()const=0;
};

class PoseOut
{
public:
	virtual void setPosition(const Pose::pos2 &pos)=0;
	virtual void setDirection(const Pose::vec2 &dir)=0;
	virtual void setPose(const Pose &pose)=0;
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
	virtual int writeState(IO::StreamOut &buffer)=0;
	virtual int readState(IO::StreamIn &buffer)=0;	
};


#include "fxObjects.h"

template<class Type> struct TypeInfo{};

class Unit;
class ObjectManager;
typedef float Damage;

class GameObject;
/// Polygon description for box2d body. Only maximum of 8 vertices is permitted
struct PolyDesc
{
	std::vector<vec2f> points;

	void addPoint(float x,float y);
	void addPoint(const vec2f & point);
};

typedef Geom::_Sphere<vec2f> Sphere2;
typedef Geom::_AABB<vec2f> AABB2;
/// description for box2d body
class BodyDesc: public Referenced, public b2BodyDef
{
public:
	//struct FixtureDef: public b2FixtureDef {};
	typedef b2FixtureDef FixtureDef;
	std::vector<FixtureDef> fixtures;
	b2BlockAllocator allocator;
	ObjectManager * manager;
	bool frozen;

	BodyDesc(ObjectManager * manager);  
	~BodyDesc();
	BodyDesc(const BodyDesc & desc);

	b2Body * create();
	float mass()const;
	bool isEmpty()const;  /// if result body is empty

	b2FixtureDef & addBox(float width,float height,float density);
	b2FixtureDef & addSphere(float size,float density);
	b2FixtureDef & addPolygon(const PolyDesc &desc,float density);
	const AABB2& getOOBB();
	const Sphere2& getBoundingSphere();
	BodyDesc & operator = (const BodyDesc & body);
protected:  
	b2FixtureDef & addFixture(float density, b2Shape & shape);	/// create new fixture
	void updateBounds();	/// update bounds

	bool bounds;			/// if bounds are updated
	AABB2 boundBox;
	Sphere2 boundSphere;
private:
	//
	void assign(const BodyDesc & desc);
};

// Contains Box2D solid body
class SolidObject: virtual public PoseIn,public PoseOut
{
protected:
	Pose pose;    /// cached pose	
	b2Body * solid;
	void updateSolidPose();  /// update solid body position
public:
	SolidObject();
	virtual ~SolidObject();
	bool isSolid()const;  /// if object has box2d body attached
	/// box2d properties
	void detachBody();
	void attachBody(b2Body * s);
	const b2Body  * getBody() const;
	b2Body  * getBody();
	const Pose::pos2 & getPosition() const;
	Pose::vec2 getDirection() const;
	void setPosition(const Pose::pos2 &pos);
	void setDirection(const Pose::vec2 &dir);
	void setPose(const Pose &pose);
	Pose getPose() const;

	Pose::vec2 getVelocityLinear() const
	{
		const b2Body * body = getBody();
		if(body)
		{
			return conv(body->GetLinearVelocity());
		}
		else
			return Pose::vec::zero();
	}
	Pose::vec3 getVelocityLinear3() const
	{
		const b2Body * body = getBody();
		if(body)
		{
			vec2f velocity2 = conv(body->GetLinearVelocity());
			return Pose::vec3(velocity2[0], velocity2[1], 0);
		}
		else
			return Pose::vec3::zero();
	}
	// sync cached pose with solid
	void syncPose();
};

// Basic definition for game object. 
class GameObjectDef: virtual public LuaObject, virtual public Referenced
{
public:
	typedef GameObject RootObject;
	typedef GameObjectDef RootObjectDef;
	typedef std::map<size_t, GameObjectDef * > Container;	
	friend class GameObject;
	friend class ObjectManager;

	size_t localID;				/// object id	
	Container::iterator back;	/// back link for fast remove
	std::string name;			/// do we need this name?

//	FxModel::Pointer model;
	FxPointer fxIdle;	
	FxPointer fxDie;
	//WeakPtr<BodyDesc> body;
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
	void addRef();			/// increase population
	void decRef();			/// decrease population	

	void attach();
	void detach();			/// detach from storage
private:
	size_t population;		/// current population	
};

typedef float Health;

class GameObject: public SolidObject, public NetObject,virtual public LuaObject, virtual public Referenced
{
public:
	typedef GameObject RootObject;
	typedef GameObjectDef RootObjectDef;
	friend class ObjectManager;
	friend class ObjectDef;
	friend class ObjStore<GameObject>;
	struct IDamageListener
	{
		virtual void onDamage(const Damage &dmg)=0;
	};
protected:	
	Health health;					/// current health
	GameObjectDef* definition;
	CollisionGroup collisionGroup;
public:
	FxHolder effects;
//	FxModel *model;
	IDamageListener *onDamage;
	int player;				/// the player owning this object

	GameObject(ObjectManager *parent,GameObjectDef* def);
	virtual ~GameObject();

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	virtual GameObjectDef * getDefinition() { return definition; };
	virtual ObjectManager * manager() {	return definition? definition->manager:NULL; }
	virtual ObjectType getType() const { return typeVoid; };
	_Scripter * getScripter() { return definition ? definition->getScripter() : NULL; }

	void makeUnique();	/// create new definition
	bool isUnique()const;

	float getSphereSize() const
	{
		return getBoundingSphere().radius;
	}

	virtual Sphere2 getBoundingSphere()const;
	virtual AABB2	getOOBB()const;			// get object oriented bounding box
	
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

	virtual int writeState(IO::StreamOut &buffer);
	virtual int readState(IO::StreamIn &buffer);
	
	//virtual _AABB<Pose::vec> getAABB()const=0;
	size_t id() const{return localID;}
protected:	
	void attach();			/// attach to storage
	void detach();			/// detach from storage
	size_t localID;			/// object id
	typedef std::map<size_t,GameObject * > Container;
	Container::iterator back;/// back link for fast remove
};

typedef SharedPtr<GameObject> GameObjectPtr;

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

b2Body * createSolidBox(ObjectManager *m,float width,float height,float mass);
b2Body * createSolidSphere(ObjectManager *m,float size,float mass);

void createObjectBox(GameObject * object, float width,float height,float mass);
void createObjectSphere(GameObject *object,float size,float mass);

void localPrintf(const char *msg);
#endif