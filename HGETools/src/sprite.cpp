#include "fxObjects.h"

namespace Fx
{
///////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////
FxSprite::FxSprite(FxManager *manager, FxTextureId texture, float texx, float texy, float w, float h)
	:Entity(manager), sprite(texture, texx, texy, w, h), scale_h(1.f)
{
}

FxSprite::FxSprite(const FxSprite &spr)
	:Base(spr), sprite(spr.sprite), scale_h(spr.scale_h)
{
}

FxSprite::FxSprite(FxManager * manager)
	:Base(manager), sprite(0, 0, 0, 0, 0), scale_h(1.f)
{
}

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
	if(!valid())
		return;
	sprite.SetBlendMode(mode|sprite.GetBlendMode());
}

void FxSprite::setBlendMode(int mode)
{
	if(!valid())
		return;
	sprite.SetBlendMode(mode);
}

void FxSprite::flipHor()
{
	bool flipX = false, flipY = false;
	sprite.GetFlip(&flipX, &flipY);
	sprite.SetFlip(!flipX, flipY);
}

Rect FxSprite::getLocalRect() const
{
	Rect result;
	math3::vec2f pos = getPose().getPosition();
	result.x1 = pos[0] - sprite.GetWidth() / 2;
	result.x2 = pos[0] + sprite.GetWidth() / 2;
	result.y1 = pos[1] - sprite.GetHeight() / 2;
	result.y2 = pos[1] + sprite.GetHeight() / 2;
	return result;
}

void FxSprite::flipVer()
{
	bool flipX = false, flipY = false;
	sprite.GetFlip(&flipX, &flipY);
	sprite.SetFlip(flipX, !flipY);
}

void drawSprite(HGE *hge, SpriteData* sprite,const Pose& p, float width, float height, bool rect=false);

void FxSprite::render(FxManager * manager, const Pose& base)
{
	if(!valid() || !visible)return;
	Pose p = base*getPose();

	int w = sprite.GetWidth();
	int h = sprite.GetHeight();	
	drawSprite(nullptr, &sprite, p, w*0.5*scale, h*0.5*scale_h*scale, sprite.GetTexture() == 0);
}

}
