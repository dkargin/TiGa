#include "fxobjects.h"
#include "rendercontext.h"

namespace Fx
{
///////////////////////////////////////////////////////
// Sprite
///////////////////////////////////////////////////////
FxSprite::FxSprite(FxManager *manager, FxTextureId texture, float texx, float texy, float w, float h)
	:sprite(texture, texx, texy, w, h), scale_h(1.f)
{
}

FxSprite::FxSprite(const FxSprite &spr)
	:Entity(spr), sprite(spr.sprite), scale_h(spr.scale_h)
{
}

FxSprite::FxSprite(FxManager * manager)
	:sprite(0, 0, 0, 0, 0), scale_h(1.f)
{
}

FxSprite::~FxSprite()
{
	// TODO: release acquired texture object
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

void FxSprite::render(RenderContext* context, const Pose& base)
{
	if(!valid() || !visible)return;
	Pose p = base*getPose();

	int w = sprite.GetWidth();
	int h = sprite.GetHeight();	
	context->drawSprite(&sprite, p, w*0.5*scale, h*0.5*scale_h*scale, sprite.GetTexture() == 0);
}

}
