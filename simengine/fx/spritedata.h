#pragma once

#include "rect.h"
#include "basetypes.h"

namespace Fx
{

/**
 * Plain sprite data
 */
class SpriteData
{
public:
	SpriteData(FxTextureId tex, int texWidth, int texHeight, float x, float y, float w, float h);

	SpriteData(const SpriteData &spr) = default;
	SpriteData() = default;

	void setTexture(FxTextureId tex, int texWidth, int texHeight);
	void setTextureRect(float x, float y, float w, float h, bool adjSize = true);
	void setColor(FxRawColor col);
	void setZ(float z);
	void setBlendMode(int blend);
	void setHotSpot(float x, float y);

	FxTextureId getTexture() const;
	Rect getTextureRect() const;
	FxRawColor getColor() const;
	float getZ() const;
	int getBlendMode() const;
	void getHotSpot(float *x, float *y) const;

	// This should be 'screen' size of a sprite
	float getWidth() const;
	float getHeight() const;

	Rect* getBoundingBox(float x, float y, Rect *rect) const;
	Rect* getBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,  Rect *rect) const;

protected:
	// Blending mode
	int blend = 0;
	// Maybe we should move color away.
	FxRawColor color;
	FxTextureId tex;
	// Relative texture coordinates, in range [0..1]
	Rect src;
	// Size of source image
	int texWidth = 0, texHeight = 0;
	// Hotspot. Used as center for rotation
	float hotX = 0.f, hotY = 0.f;
	// Z-offset of this sprite
	float z = 0.f;
};

}
