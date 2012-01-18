#include "stdafx.h"
#include "fxObjects.h"

///////////////////////////////////////////////////////
// Basic effect
///////////////////////////////////////////////////////
FxEffect::FxEffect(FxManager * manager):manager(manager),visible(true),scale(1.0f), cloned(false)//,xOrg(0),yOrg(0)
{
	//logger()->line(0,"FxEffect()");
}

FxEffect::FxEffect(const FxEffect &effect)
	:manager(effect.manager),visible(effect.visible),pose(effect.pose),scale(effect.scale),cloned(true)//,xOrg(effect.xOrg),yOrg(effect.yOrg)
{
	//logger()->line(0,"FxEffect(const FxEffect &)");
}

FxEffect::~FxEffect()
{	
	//logger()->line(0,"~FxEffect()");	
}

Log * FxEffect::logger() const
{
	if( manager )
		return manager->logger();
	else
		return NULL;
}

void FxEffect::setPose(const FxEffect::Pose::pos3 &v,const FxEffect::Pose::rot &r)
{
	pose.position = v;
	pose.orientation=r;
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

FxEffect::Pose FxEffect::getPose() const
{
	return pose;
}

FxEffect::Pose FxEffect::getGlobalPose()const
{
	return parent ? parent->getGlobalPose() * pose : pose;
}

void FxEffect::start()
{
	if(!valid())
		return;
}

void FxEffect::query(const FxEffect::Pose & base)
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
	:sprite(tex,x,y,w,h),FxEffect(manager),scale_h(1.f)
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
	sprite.SetBlendMode(mode|sprite.GetBlendMode());
}

void FxSprite::setBlendMode(int mode)
{
	if(!valid())return;
	sprite.SetBlendMode(mode);
}

void drawSprite(HGE *hge,hgeSprite *sprite,const FxEffect::Pose &p,float width,float height,bool rect=false);

void FxSprite::render(const Pose &base)
{
	if(!valid() || !visible)return;
	Pose p=base*getPose();
	int w = sprite.GetWidth();
	int h = sprite.GetHeight();	
	drawSprite(manager->hge,&sprite,p,w*0.5*scale,h*0.5*scale_h*scale,false);
}

///////////////////////////////////////////////////////
// Sound
///////////////////////////////////////////////////////
FxSound::FxSound(FxManager * manager, HGE *hge,const char *file)
	:sound(hge->Effect_Load(file)),hge(hge),name(file),FxEffect(manager), channel(0)
{
	//_RPT0(0,"FxSound::FxSound()\n");
}

FxSound::FxSound(const FxSound &effect)
	:name(effect.name),hge(effect.hge),FxEffect(effect)
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
// всё к чему можно прицепить спецэффект
//////////////////////////////////////////////////////////////////////////
FxHolder::FxHolder(FxManager * manager)
	:FxEffect(manager)
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
		FxEffect * effect = begin().get();
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