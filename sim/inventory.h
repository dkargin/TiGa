#pragma once


#include "gameObject.h"

namespace sim
{
class Item;
class ObjectManager;
typedef ObjectManager ItemManager;
class Unit;

#ifdef FUCK_THIS
class ItemDef
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
	Item * construct(StreamIn *context);
	GameObject * create(StreamIn * buffer);
protected:
	ItemDef(const ItemDef &def);
};

#endif

enum class ItemStatus
{
	Void,							//< Item is not attached to the physical world
	InInventory,			//< Item is stored in some inventory
	OnGround,					//< Item is left on ground
};

class Item : public GameObject
{
public:
	int stack;			// number of items in stack

	// From ItemDesc
	std::string itemType;
	int maxStack;
	int maxHealth;

	ItemStatus state;

	float size;			// all items are square shape
public:
	Item(ItemManager* im, Item* proto);
	~Item();

	int writeDesc(IOBuffer &context);
	int readDesc(IOBuffer &context);
	
	virtual void update(float dt);

	CollisionGroup getCollisionGroup() const
	{
		return cgBody;
	}

private:
	Item* prototype;
};

/// Contains a set of items
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
	Inventory(Unit *own, const Pose &offs);
	virtual ~Inventory();
	virtual void addItem(Item *item);
	virtual void removeItem(Item *item);
	virtual bool hasItem(Item *item);
	virtual void update(float dt);

	virtual void save(StreamOut & stream);
	virtual void load(StreamIn & stream);
};

}
