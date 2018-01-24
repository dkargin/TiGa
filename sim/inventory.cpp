#include "../sim/inventory.h"

#include "../sim/device.h"
#include "../sim/objectManager.h"
#include "../sim/unit.h"
#include "stdafx.h"
//#include "world.h"
//#include "globals.h"
///////////////////////////////////////////////////////////////////////////////
// ItemDef
///////////////////////////////////////////////////////////////////////////////
ItemDef::ItemDef(ObjectManager *owner)
:size(1.0f),maxStack(1),maxHealth(1),GameObjectDef(owner)
{}

ItemDef::ItemDef(const ItemDef &def)
	:GameObjectDef(def)
{
	maxHealth = def.maxHealth;
	maxStack = def.maxHealth;
	itemType = def.itemType;
}

ItemDef::~ItemDef() {}

GameObject * ItemDef::create(IO::StreamIn * buffer)
{
	return construct(buffer);
}

Item * ItemDef::construct(IO::StreamIn *context)
{
	b2Body * solid=NULL;
	if(context && !context->eof())	// retrieve b2Body ID from context
	{
		//b2BodyDef def;
		//context->read(def.id);
		//b2Body=manager->scene->createb2Body(def);
	}
	b2BodyDef bodyDef;
	bodyDef.type=b2_dynamicBody;
	bodyDef.bullet=true;
	bodyDef.position.Set(0.0f, 0.0f);
	solid=manager->getDynamics()->CreateBody(&bodyDef);
	//b2CircleShape shape;
	//shape.m_radius=1;

	b2PolygonShape shape;
	shape.SetAsBox(size, size);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	solid->CreateFixture(&fixtureDef);
	
	Item *result=NULL;
	if(solid)
	{
		result=new Item(this);
		result->attachBody(solid);
	}	
	return result;
}
///////////////////////////////////////////////////////////////////////////////
// Item
///////////////////////////////////////////////////////////////////////////////
Item::Item(ItemDef *def)
:state(Item::placeLand),GameObject(def->manager,def)
{
	if(def->fxIdle)
	{
		effects = def->fxIdle->clone();
		//effects.attach(def->fxIdle->clone());
	}
}
Item::~Item()
{}
void Item::update(float dt)
{
  GameObject::update(dt);
}

int Item::writeDesc(IOBuffer &context)
{
	//ID id=b2Body?b2Body->getID():-1;
	//context->write(id);
	//xwrite(*context,id);
	return 0;
}
int Item::readDesc(IOBuffer &context)
{
	//ID id=b2Body?b2Body->getID():-1;
	//context->write(id);
	//xwrite(*context,id);
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
// Inventory
///////////////////////////////////////////////////////////////////////////////
Inventory::Inventory(Unit *o,const Pose &offs)
:owner(o),offset(offs)
{
	destroyItems=false;
	drawInventory=true;
}
	
Inventory::~Inventory()
{
	for(std::list<Item*>::iterator it=items.begin();it!=items.end();++it)
	{
		Item *item=*it;
		// destroy or drop?
		//if(destroyItems)
		//	item->release();
		//else
		//item->state=Item::placeLand;	
	}
}
void Inventory::addItem(Item *item)
{
	items.push_back(item);
}
void Inventory::removeItem(Item *item)
{
	for(Container::iterator it=items.begin();it!=items.end();++it)
		if(*it==item)
		{
			items.erase(it);
			break;
		}
}
bool Inventory::hasItem(Item *item)
{
	for(Container::iterator it=items.begin();it!=items.end();++it)
		if(*it==item)
			return true;
	return false;
}
void Inventory::update(float dt)
{	
	for(std::list<Item*>::iterator it=items.begin();it!=items.end();++it)
	{
		Item *item=*it;
		//Mt4x4 pose=owner->b2Body->getPose();
		//vec2f c=owner->getPose()*offset;
		//pose.origin(c);
		//item->setPosition(c);
	}
}

void Inventory::save(IO::StreamOut & stream)
{
}

void Inventory::load(IO::StreamIn & stream)
{
}
////////////////////////////////////////////////////////////////////////////////
// ItemManager
////////////////////////////////////////////////////////////////////////////////
//ItemManager::ItemManager(b2World *s)
//	:dynamics(s)
//{}
//Item *ItemManager::newObject(ItemDef *def)
//{		
//	return def->create();
//}
//ItemDef *ItemManager::newDefinition()
//{
//	return new ItemDef(this);
//}
//
//void ItemManager::update(float dt)
//{
//	for(Objects::iterator it=objects.begin();it!=objects.end();++it)
//		(*it)->update(dt);
//}
