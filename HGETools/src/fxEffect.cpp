#include "stdafx.h"
#include "fxObjects.h"

///////////////////////////////////////////////////////
// Basic effect
///////////////////////////////////////////////////////
FxEffect::FxEffect(FxManager * manager) : manager(manager->shared_from_this())
{	
	scale = 1.f;
	visible = true;	
}

FxEffect::FxEffect(const FxEffect &effect)	
{
	manager = effect.manager;
	visible = effect.visible;
	scale = effect.scale;
	pose = effect.pose;

	for(const_iterator it = effect.begin(); it != effect.end();++it)
		attach(it->clone());
}

FxEffect::~FxEffect()
{		
}

FxManager * FxEffect::getManager() const
{
	return manager.lock().get();
}

Log * FxEffect::logger() const
{
	if( getManager() )
		return getManager()->logger();
	else
		return NULL;
}

HGE * FxEffect::hge() const
{
	if( getManager() )
		return getManager()->hge;
	else
		return NULL;
}

void FxEffect::setPose(const FxEffect::Pose::pos &v,const FxEffect::Pose::rot &r)
{
	pose.setPosition(v);
	pose.orientation = r;
}

void FxEffect::setScale(float s)
{
	scale = s;
}

float FxEffect::getScale() const
{
	return scale;
}

void FxEffect::setPose(const FxEffect::Pose &p)
{
	pose=p;
}

void FxEffect::setPose(float x, float y, float rot)
{
	pose.position[0] = x;
	pose.position[1] = y;
	pose.orientation = rot;
}

void FxEffect::setZ(float z)
{
	pose.position[2] = z;
}

float FxEffect::getZ() const
{
	return pose.position[2];
}

const FxEffect::Pose & FxEffect::getPose() const
{
	return pose;
}

FxEffect::Pose FxEffect::getGlobalPose()const
{
	return parent ? parent->getGlobalPose() * pose : pose;
}

hgeRect FxEffect::getLocalRect() const
{
	vec2f pos = pose.getPosition();
	return hgeRect(pos[0], pos[1], pos[0], pos[1]);
}

bool FxEffect::valid() const
{
	return true;
}

float FxEffect::duration()const
{
	float result = 0;
	for(const_iterator it = begin(); it != end(); ++it)
	{
		result = std::max(result, it->duration());
	}
	return result;
}

float FxEffect::getWidth() const
{	
	return getClipRect().width();
}

float FxEffect::getHeight() const
{
	return getClipRect().height();
}

void FxEffect::start(AnimationMode mode)
{
	for(iterator it = begin();it != end();++it)
		it->start(mode);
}

void FxEffect::stop(bool immediate)
{
	for(iterator it = begin(); it!=end();++it)
		it->stop(immediate);
}

void FxEffect::rewind()
{
	for(iterator it=begin();it!=end();++it)
		it->rewind();
}

// empty
void FxEffect::update(float dt) {}

void FxEffect::updateAll(float dt)
{
	for(iterator it=begin();it!=end();++it)
		it->update(dt);
}

// empty
void FxEffect::render(FxManager * manager, const Pose &base) {}

void FxEffect::renderAll(FxManager * manager, const Pose &base)
{
	for(iterator it=begin();it!=end();++it)
		it->render(manager, base*pose);
}

void FxEffect::query(FxManager * manager, const Pose & base) 
{
	manager->renderQueue.query(base, this);	
}

void FxEffect::queryAll(FxManager * manager, const Pose & base)
{	
	// query itself
	query(manager, base);
	// query children	
	for(iterator it = begin(); it != end(); ++it)
		it->queryAll(manager, base*pose);
}

hgeRect FxEffect::getClipRect() const
{
	hgeRect result = getLocalRect();
	bool first = true;
	for(const_iterator it = begin();it != end();++it)
	{
		hgeRect childRect = it->getClipRect();
		if( first )
			result = childRect;
		else
			result = hgeRect::Merge( result, childRect );
	}
	return result;
}

bool FxEffect::shouldClip(const FxView2 & view) const
{
	// TODO:
	return false;
}

FxEffect::FxType FxEffect::type() const
{
	return fxHolder;
}

FxEffect * FxEffect::clone() const
{
	return new FxEffect(*this);
}