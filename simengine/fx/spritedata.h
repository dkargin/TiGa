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

	void SetTexture(FxTextureId tex, int texWidth, int texHeight);
	void SetTextureRect(float x, float y, float w, float h, bool adjSize = true);
	void SetColor(FxRawColor col);
	void SetZ(float z);
	void SetBlendMode(int blend);
	void SetHotSpot(float x, float y);

	FxTextureId GetTexture() const;
	Rect GetTextureRect() const;
	FxRawColor GetColor() const;
	float GetZ() const;
	int GetBlendMode() const;
	void GetHotSpot(float *x, float *y) const;

	float GetWidth() const;
	float GetHeight() const;

	Rect* GetBoundingBox(float x, float y, Rect *rect) const;
	Rect* GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,  Rect *rect) const;

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
