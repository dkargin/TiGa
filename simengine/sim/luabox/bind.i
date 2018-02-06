%module game

%{
#include "stdafx.h"
#include "messages.pb.h"
#include "Game.h"
%}

class Item
{
public:
	std::string name;
	int itemType;	// static item type. Used to check if two items are stackable
	int stackSize;	// current stack size
	int stackMax;	// max stack
	int fastSlot;	// assigned fast slot?	
	int blockType;	// block type to be spawned. Move it in LUA
	int id;			// item id in inventory

	Item * clone();
};

struct btQuaternion
{
	
};

struct btVector3
{	
	btVector3();
	btVector3(float x, float y, float z);
	~btVector3();

	void normalize ();

	float x();
	float y();
	float z();

	float length();
	float length2();
};

struct btTransform
{
	void setIdentity();
};

enum GameObjectType
{
	GameObjectDummy,
	GameObjectTerrain,
	GameObjectCharacter,
	GameObjectBuilding,
	GameObjectBullet,		// Bullet, Missile, Beam
};


class GameObject 
{
public:
	int getID() const;
	bool needsSync() const;
	void markSync(bool flag);

	void setPosition(float x, float y, float z);
	void setTransform(const btTransform & transform);
	btTransform getTransform();
};

class Building : public GameObject
{
public:
	Block *setLocalBlock(int x, int y, int z, int type);
	Block *getLocalBlock(int x, int y, int z);
	bool raycastLocal(const btVector3 &from, const btVector3 &to, TargetBlock &block, TargetBlock &build);
};

class Character : public GameObject
{
public:
	void resetItemSlots();
	void removeAllItems();

	void useItemSelf();
	void useItemTarget();

	void assignItemSlot(int slot, int inventoryIndex);
};

class Game
{
public:
	// find player by name
	int findPlayerName(const char * name) const;
	// get current frame
	int getFrame() const;
	bool playerValid(int player) const;
	void linkObject(GameObject * object);

	Building * createBuilding();
	Character * createCharacter();
};

Game * GetGameInstance();

%{

bool LuaBox::pushObjectPtr(lua_State *L,void *object,const char *name)
{
	static char mangled[255];
	strcpy(mangled,"_p_");
	strcpy(mangled+3,name);
	swig_type_info *type=SWIG_TypeQuery(mangled);
	if(!type)return false;
	SWIG_Lua_NewPointerObj(L,object,type,0);
	return true;	
}
%}
