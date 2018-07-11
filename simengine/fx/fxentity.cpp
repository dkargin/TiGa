#include "fxobjects.h"

namespace Fx
{

Entity::Entity()
{
	scale = 1.f;
	visible = true;
}

Entity::Entity(const Entity& effect)
{
	visible = effect.visible;
	scale = effect.scale;
	pose = effect.pose;

	for(EntityPtr ptr: children)
	{
		EntityPtr clone(ptr->clone());
		attach(clone);
	}
}

Entity::~Entity()
{		
}

void Entity::setPose(const Pose::pos &v,const Pose::rot &r)
{
	pose.setPosition(v);
	pose.orientation = r;
}

void Entity::setScale(float s)
{
	scale = s;
}

float Entity::getScale() const
{
	return scale;
}

void Entity::setPose(const Pose &p)
{
	pose=p;
}

void Entity::setPose(float x, float y, float rot)
{
	pose.position[0] = x;
	pose.position[1] = y;
	pose.orientation = rot;
}

void Entity::setZ(float z)
{
	pose.position[2] = z;
}

float Entity::getZ() const
{
	return pose.position[2];
}

const Pose & Entity::getPose() const
{
	return pose;
}

Pose Entity::getGlobalPose()const
{
	EntityPtr parent_ptr = parent.lock();
	return parent_ptr ? parent_ptr->getGlobalPose() * pose : pose;
}

Rect Entity::getLocalRect() const
{
	auto pos = pose.getPosition();
	return Rect(pos[0], pos[1], pos[0], pos[1]);
}

bool Entity::valid() const
{
	return true;
}

void Entity::setVisible(bool flag)
{
	visible = flag;
}

bool Entity::isVisible() const
{
	return visible;
}

float Entity::duration()const
{
	float result = 0;
	for(EntityPtr ptr: children)
	{
		result = std::max(result, ptr->duration());
	}
	return result;
}

float Entity::getWidth() const
{	
	return getClipRect().width();
}

float Entity::getHeight() const
{
	return getClipRect().height();
}

EntityPtr Entity::sharedMe()
{
	return shared_from_this();
}

void Entity::start(AnimationMode mode)
{
	for(EntityPtr ptr: children)
			ptr->start(mode);
}

void Entity::stop(bool immediate)
{
	for(EntityPtr ptr: children)
		ptr->stop(immediate);
}

void Entity::rewind()
{
	for(EntityPtr ptr: children)
		ptr->rewind();
}

// empty
void Entity::update(float dt) {}

void Entity::updateAll(float dt)
{
	for(EntityPtr ptr: children)
		ptr->update(dt);
}

void Entity::render(RenderContext* manager, const Pose& base)
{
	// empty
}

void Entity::renderAll(RenderContext* manager, const Pose& base)
{
	for(EntityPtr ptr: children)
		ptr->render(manager, base*pose);
}

#ifdef FUCK_THIS_LESS
void Entity::query(FxManager * manager, const Pose & base)
{
	manager->renderQueue.query(base, this);	
}

void Entity::queryAll(FxManager * manager, const Pose & base)
{	
	// query itself
	query(manager, base);
	// query children	
	for(iterator it = begin(); it != end(); ++it)
		it->queryAll(manager, base*pose);
}
#endif

Rect Entity::getClipRect() const
{
	Rect result = getLocalRect();
	bool first = true;

	for(EntityPtr ptr: children)
	{
		Rect childRect = ptr->getClipRect();
		if( first )
			result = childRect;
		else
			result = Rect::merge( result, childRect );
	}
	return result;
}

bool Entity::shouldClip(const FxView2 & view) const
{
	// TODO:
	return false;
}

EffectType Entity::type() const
{
	return EffectType::EffectGroup;
}

EntityPtr Entity::clone() const
{
	return EntityPtr(new Entity(*this));
}

void Entity::detach_me()
{
	EntityPtr parent_ptr = parent.lock();
	if(!parent_ptr)
		return;

	parent_ptr->children.erase(this->parent_it);
	parent.reset();
}

void Entity::attach(EntityPtr obj)
{
	EntityPtr thisptr = shared_from_this();
	if (thisptr == obj->parent.lock())
		return;

	obj->detach_me();
	obj->parent_it = children.insert(children.end(), obj);
	obj->parent = thisptr;
}

void Entity::onAttach(Entity* other)
{

}

void Entity::onDetach(Entity* other)
{

}
}
