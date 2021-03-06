#ifndef GAME_OBJECT
#define GAME_OBJECT

//#include "ioTools.h"

#pragma once
#pragma warning(disable:4250)

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
class SolidObject
{
protected:
	Pose pose;    /// cached pose	
	b2Body * solid;
	void updateSolidPose();  /// update solid body position
public:
	SolidObject();
	virtual ~SolidObject();

	bool isSolid()const;  /// if object has box2d body attached
	/// box2d handling
	void detachBody();
	void attachBody(b2Body * s);
	const b2Body  * getBody() const;
	b2Body  * getBody();	
	void syncPose();	// sync cached pose with solid

	Pose::vec getVelocityLinear() const;
	Pose::vec3 getVelocityLinear3() const;
	/// Pose
	const Pose::pos & getPosition() const;
	Pose::vec getDirection() const;
	void setPosition(const Pose::pos &pos);
	void setDirection(const Pose::vec &dir);
	void setPose(const Pose &pose);
	Pose getPose() const;	
	/// Geometry
	Sphere2 getBoundingSphere()const;
	AABB2	getOOBB()const;			// get object oriented bounding box	
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

	FxEffect::Pointer effects;
//	FxModel *model;
	IDamageListener * onDamage;
	int player;				/// the player owning this object
protected:	
	Health health, maxHealth;		/// current health
	GameObjectDef * definition;		/// object definition
	CollisionGroup collisionGroup;	/// current collison group
	ObjectManager * manager;		/// where is it stored
	GameObject * objectPrev, * objectNext;
public:
	

	GameObject(ObjectManager *parent, GameObjectDef* def);
	virtual ~GameObject();

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);

	GameObject * getNext();
	virtual GameObjectDef * getDefinition();
	virtual ObjectManager * getManager();
	virtual ObjectType getType() const;
	virtual _Scripter * getScripter();

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
	size_t localID;			/// object id	
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

//void createObjectBox(GameObject * object, float width,float height,float mass);
//void createObjectSphere(GameObject *object, float size,float mass);

//void localPrintf(const char *msg);
#endif