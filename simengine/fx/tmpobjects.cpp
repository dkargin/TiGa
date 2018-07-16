#include <fxobjects.h>

#include "stdafx.h"

namespace Fx
{

Entity::Entity(FxManager * manager):manager(manager),visible(true),scale(1.0f), cloned(false)//,xOrg(0),yOrg(0)
{
	//logger()->line(0,"Entity()");
}

Entity::Entity(const Entity &effect)
	:manager(effect.manager),visible(effect.visible),pose(effect.pose),scale(effect.scale),cloned(true)//,xOrg(effect.xOrg),yOrg(effect.yOrg)
{
	//logger()->line(0,"Entity(const Entity &)");
}

Entity::~Entity()
{	
	//logger()->line(0,"~Entity()");
}

Log * Entity::logger() const
{
	if( manager )
		return manager->logger();
	else
		return NULL;
}

void Entity::setPose(const Entity::Pose::pos3 &v,const Entity::Pose::rot &r)
{
	pose.position = v;
	pose.orientation=r;
}

void Entity::setScale(float s)
{
	scale = s;
}

float Entity::getScale() const
{
	return scale;
}

void Entity::setPose(const Entity::Pose &p)
{
	pose=p;
}

Entity::Pose Entity::getPose() const
{
	return pose;
}

Entity::Pose Entity::getGlobalPose()const
{
	return parent ? parent->getGlobalPose() * pose : pose;
}

void Entity::start()
{
	if(!valid())
		return;
}

void Entity::query(const Entity::Pose & base)
{
	if(manager)
		manager->renderQueue.query(base,this);
}
///////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////
/*FxSprite::FxSprite(hgeResourceManager &manager,const char *name)
	:sprite(manager.GetSprite(name))
{
	//g_logger->line(0,"FxSprite::FxSprite()");
	//_RPT0(0,"FxSprite::FxSprite()\n");
}*/
FxSprite::FxSprite(FxManager *manager,HTEXTURE tex, float x, float y, float w, float h)
	:sprite(tex,x,y,w,h),Entity(manager),scale_h(1.f)
{}

FxSprite::FxSprite(const FxSprite &spr)
	:Base(spr),sprite(spr.sprite),scale_h(spr.scale_h)
{
	
	//g_logger->line(0,"FxSprite::FxSprite(const FxSprite &)");
	//_RPT0(0,"FxSprite::FxSprite(const FxSprite &spr)\n");
}

FxSprite::~FxSprite()
{
	//g_logger->line(0,"FxSprite::~FxSprite()");
	//_RPT0(0,"FxSprite::~FxSprite()\n");
	/*if(sprite && cloned)
	{
		delete sprite;
		sprite=NULL;
	}*/
}

bool FxSprite::valid() const
{
	return true;
}

void FxSprite::update(float dt)
{
	//sprite->SetHotSpot(xOrg,yOrg);
}

void FxSprite::addBlendMode(int mode)
{
	if(!valid())return;
	sprite.setBlendMode(mode|sprite.getBlendMode());
}

void FxSprite::setBlendMode(int mode)
{
	if(!valid())return;
	sprite.setBlendMode(mode);
}

void drawSprite(HGE *hge,hgeSprite *sprite,const Entity::Pose &p,float width,float height,bool rect=false);

void FxSprite::render(const Pose &base)
{
	if(!valid() || !visible)return;
	Pose p=base*getPose();
	int w = sprite.getWidth();
	int h = sprite.getHeight();	
	drawSprite(manager->hge,&sprite,p,w*0.5*scale,h*0.5*scale_h*scale,false);
}

#ifdef FUCK_THIS
///////////////////////////////////////////////////////
// Sound
///////////////////////////////////////////////////////
FxSound::FxSound(FxManager * manager, HGE *hge,const char *file)
	:sound(hge->Effect_Load(file)),hge(hge),name(file),Entity(manager), channel(0)
{
	//_RPT0(0,"FxSound::FxSound()\n");
}

FxSound::FxSound(const FxSound &effect)
	:name(effect.name),hge(effect.hge),Entity(effect)
{
	//_RPT0(0,"FxSound::FxSound(const FxSound &spr)\n");
	sound = hge->Effect_Load(name.c_str());
}

FxSound::~FxSound()
{
	//_RPT0(0,"FxSound::~FxSound()\n");
	hge->Effect_Free(sound);
}

void FxSound::start()
{
	channel = hge->Effect_Play(sound);	
}

void FxSound::stop()
{
	if(channel)
	{
		hge->Channel_Stop(channel);
		channel = 0;
	}
}

void FxSound::update(float dt)
{
	//float length = (getPose()).position.length();
	//const float distanceScale = 10;
	//float volume = 100 / 100 + distanceScale * length;
	//hge->Channel_SetVolume(channel, volume);
}

void FxSound::render(const Pose &base)
{
	
}

bool FxSound::valid()const
{
	return sound!=NULL;
}

//////////////////////////////////////////////////////////////////////////
// FxHolder
// �� � ���� ����� ��������� ����������
//////////////////////////////////////////////////////////////////////////
FxHolder::FxHolder(FxManager * manager)
	:Entity(manager)
{
	//_RPT0(0,"FxHolder::FxHolder()\n");
}

FxHolder::FxHolder(const FxHolder &effect)
	:Base(effect)
{
	//_RPT0(0,"FxHolder::FxHolder(const FxHolder &spr)\n");
	for(const_iterator it = effect.begin(); it != effect.end();++it)
		attach(it->clone());
}

FxHolder::~FxHolder()
{
	//_RPT0(0,"FxHolder::~FxHolder()\n");
	/*
	while(!empty())
	{
		Entity * effect = begin().get();
		if(effect != NULL)
			delete begin().get();
		else
			erase(begin());
	}
	clear();*/
}

float FxHolder::duration()const
{
	float result = 0;
	for(const_iterator it = begin(); it != end(); ++it)
	{
		result = std::max(result, it->duration());
	}
	return result;
}

bool FxHolder::valid() const
{
	return true;
}

void FxHolder::start()
{
	for(iterator it = begin();it != end();++it)
		it->start();
}

void FxHolder::stop()
{
	for(iterator it = begin(); it!=end();++it)
		it->stop();
}

void FxHolder::rewind()
{
	for(iterator it=begin();it!=end();++it)
		it->rewind();
}

void FxHolder::update(float dt)
{
	for(iterator it=begin();it!=end();++it)
		it->update(dt);
}

void FxHolder::render(const Pose &base)
{
	for(iterator it=begin();it!=end();++it)
		it->render(base*pose);
}

void FxHolder::query(const Pose & base)
{
	for(iterator it=begin();it!=end();++it)
		it->query(base*pose);
}
#endif

} //namespace Fx
