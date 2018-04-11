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
	SpriteData(FxTextureId tex, float x, float y, float w, float h);
	SpriteData(const SpriteData &spr);

	~SpriteData();
	
	/*
	/// Should be moved to batch->render
	void		Render(float x, float y);
	void		RenderEx(float x, float y, float rot, float hscale=1.0f, float vscale=0.0f);
	void		RenderStretch(float x1, float y1, float x2, float y2);
	void		Render4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
	*/

	void SetTexture(FxTextureId tex);
	void SetTextureRect(float x, float y, float w, float h, bool adjSize = true);
	void SetColor(FxRawColor col);
	void SetZ(float z);
	void SetBlendMode(int blend);
	void SetHotSpot(float x, float y);
	void SetFlip(bool bX, bool bY, bool bHotSpot = false);

	FxTextureId GetTexture() const;
	void GetTextureRect(float *x, float *y, float *w, float *h) const;
	FxRawColor GetColor() const;
	float GetZ() const;
	int GetBlendMode() const;
	void GetHotSpot(float *x, float *y) const;
	void GetFlip(bool *bX, bool *bY) const;

	float GetWidth() const;
	float GetHeight() const;

	Rect* GetBoundingBox(float x, float y, Rect *rect) const { rect->Set(x-hotX, y-hotY, x-hotX+width, y-hotY+height); return rect; }
	Rect* GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,  Rect *rect) const;

protected:
	SpriteData();

	int blend;
	FxTextureId tex;
	FxRawColor color;
	float tx, ty, width, height;
	float tex_width, tex_height;
	float hotX, hotY;
	float z;
	bool bXFlip, bYFlip, bHSFlip;
};

}
