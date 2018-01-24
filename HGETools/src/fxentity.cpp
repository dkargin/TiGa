#include "fxobjects.h"

namespace Fx
{

Entity::Entity()
{	
	scale = 1.f;
	visible = true;	
}

Entity::Entity(const Entity &effect)
{
	visible = effect.visible;
	scale = effect.scale;
	pose = effect.pose;

	for(const_iterator it = effect.begin(); it != effect.end();++it)
	{
		Entity* clone = it->clone();
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
	return parent ? parent->getGlobalPose() * pose : pose;
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
	for(const_iterator it = begin(); it != end(); ++it)
	{
		result = std::max(result, it->duration());
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

void Entity::start(AnimationMode mode)
{
	for(iterator it = begin();it != end();++it)
		it->start(mode);
}

void Entity::stop(bool immediate)
{
	for(iterator it = begin(); it!=end();++it)
		it->stop(immediate);
}

void Entity::rewind()
{
	for(iterator it=begin();it!=end();++it)
		it->rewind();
}

// empty
void Entity::update(float dt) {}

void Entity::updateAll(float dt)
{
	for(iterator it=begin();it!=end();++it)
		it->update(dt);
}

void Entity::render(RenderContext* manager, const Pose& base)
{
	// empty
}

void Entity::renderAll(RenderContext* manager, const Pose& base)
{
	for(iterator it=begin();it!=end();++it)
		it->render(manager, base*pose);
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
	for(const_iterator it = begin();it != end();++it)
	{
		Rect childRect = it->getClipRect();
		if( first )
			result = childRect;
		else
			result = Rect::Merge( result, childRect );
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

Entity * Entity::clone() const
{
	return new Entity(*this);
}

}
