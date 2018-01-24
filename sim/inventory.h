#pragma once
#include "../sim/gameObject.h"
class Item;
class ObjectManager;
typedef ObjectManager ItemManager;
class Unit;

class ItemDef: public GameObjectDef
{
public:
	friend Item;
	friend ItemManager;
	std::string itemType;
	int maxStack;
	int maxHealth;

	float size;			// all items are square shape
	OBJECT_DEF(Item,typeItem);

public:
	ItemDef(ObjectManager *owner);	
	~ItemDef();
	void release();
	Item * construct(IO::StreamIn *context);
	GameObject * create(IO::StreamIn * buffer);
protected:
	ItemDef(const ItemDef &def);
};

class Item : public GameObject
{
public:
	enum Place
	{
		placeInventory,
		placeLand,
	}state;	
	int stack;			// number of items in stack
	OBJECT_IMPL(Item,typeItem);
public:
	Item(ItemDef *def);
	~Item();

	int writeDesc(IOBuffer &context);
	int readDesc(IOBuffer &context);
	
	virtual void update(float dt);

	CollisionGroup getCollisionGroup()const
	{
		return cgBody;
	}
};

class Inventory
{
public:
	Unit *owner;
	Pose offset;			// inventory offset
	bool drawInventory;		
	bool destroyItems;		// whether to destroy containing items
	typedef std::list<Item*> Container;
	Container items;
public:
	Inventory(Unit *own,const Pose &offs);
	~Inventory();
	virtual void addItem(Item *item);
	virtual void removeItem(Item *item);
	virtual bool hasItem(Item *item);
	virtual void update(float dt);

	virtual void save(IO::StreamOut & stream);
	virtual void load(IO::StreamIn & stream);
};