#include "stdafx.h"
#include "fxObjects.h"

///////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////
FxSprite::FxSprite(FxManager *manager,HTEXTURE tex, float x, float y, float w, float h)
	:sprite(tex,x,y,w,h),FxEffect(manager),scale_h(1.f)
{}

FxSprite::FxSprite(const FxSprite &spr)
	:Base(spr),sprite(spr.sprite),scale_h(spr.scale_h)
{	
	//g_logger->line(0,"FxSprite::FxSprite(const FxSprite &)");
	//_RPT0(0,"FxSprite::FxSprite(const FxSprite &spr)\n");
}

FxSprite::FxSprite(FxManager * manager)
	:Base(manager), sprite(0, 0, 0, 0, 0), scale_h(1.f)
{}

FxSprite::~FxSprite()
{}

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

void FxSprite::flipHor()
{
	bool flipX = false, flipY = false;
	sprite.GetFlip(&flipX, &flipY);
	sprite.SetFlip(!flipX, flipY);
}

float FxSprite::getWidth() const
{
	return sprite.GetWidth();
}

float FxSprite::getHeight() const
{
	return sprite.GetHeight();
}

void FxSprite::flipVer()
{
	bool flipX = false, flipY = false;
	sprite.GetFlip(&flipX, &flipY);
	sprite.SetFlip(flipX, !flipY);
}

void drawSprite(HGE *hge,hgeSprite *sprite,const FxEffect::Pose &p,float width,float height,bool rect=false);

void FxSprite::render(const Pose &base)
{
	if(!valid() || !visible)return;
	Pose p = base*getPose();
	int w = sprite.GetWidth();
	int h = sprite.GetHeight();	
	drawSprite(manager->hge,&sprite, p, w*0.5*scale, h*0.5*scale_h*scale, sprite.GetTexture() == 0);
}