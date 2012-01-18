#include "stdafx.h"
#include "fxObjects.h"

///////////////////////////////////////////////////////
// Basic effect
///////////////////////////////////////////////////////
FxEffect::FxEffect(FxManager * manager) : manager(manager)
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

Log * FxEffect::logger() const
{
	if( manager )
		return manager->logger();
	else
		return NULL;
}

HGE * FxEffect::hge() const
{
	if( manager )
		return manager->hge;
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
const FxEffect::Pose & FxEffect::getPose() const
{
	return pose;
}

FxEffect::Pose FxEffect::getGlobalPose()const
{
	return parent ? parent->getGlobalPose() * pose : pose;
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

void FxEffect::update(float dt)
{
	for(iterator it=begin();it!=end();++it)
		it->update(dt);
}

void FxEffect::render(const Pose &base)
{
	for(iterator it=begin();it!=end();++it)
		it->render(base*pose);
}

void FxEffect::query(const Pose & base)
{
	if(manager)
		manager->renderQueue.query(base, this);
	for(iterator it=begin();it!=end();++it)
		it->query(base*pose);
}

hgeRect FxEffect::getClipRect() const
{
	hgeRect result(0,0,0,0);
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

FxEffect::Pointer FxEffect::clone() const
{
	return new FxEffect(*this);
}