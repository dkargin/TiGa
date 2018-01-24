/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hgeSprite helper class header
*/


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

	~SpriteData() {  }
	
	/*
	/// Should be moved to batch->render
	void		Render(float x, float y);
	void		RenderEx(float x, float y, float rot, float hscale=1.0f, float vscale=0.0f);
	void		RenderStretch(float x1, float y1, float x2, float y2);
	void		Render4V(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
	*/

	void		SetTexture(FxTextureId tex);
	void		SetTextureRect(float x, float y, float w, float h, bool adjSize = true);
	void		SetColor(FxRawColor col, int i=-1);
	void		SetZ(float z, int i=-1);
	void		SetBlendMode(int blend) { quad.blend=blend; }
	void		SetHotSpot(float x, float y) { hotX=x; hotY=y; }
	void		SetFlip(bool bX, bool bY, bool bHotSpot = false);

	FxTextureId	GetTexture() const { return quad.tex; }
	void		GetTextureRect(float *x, float *y, float *w, float *h) const { *x=tx; *y=ty; *w=width; *h=height; }
	FxRawColor	GetColor(int i=0) const { return quad.v[i].col; }
	float		GetZ(int i=0) const { return quad.v[i].z; }
	int			GetBlendMode() const { return quad.blend; }
	void		GetHotSpot(float *x, float *y) const { *x=hotX; *y=hotY; }
	void		GetFlip(bool *bX, bool *bY) const { *bX=bXFlip; *bY=bYFlip; }

	float		GetWidth() const { return width; }
	float		GetHeight() const { return height; }

	Rect*	GetBoundingBox(float x, float y, Rect *rect) const { rect->Set(x-hotX, y-hotY, x-hotX+width, y-hotY+height); return rect; }
	Rect*	GetBoundingBoxEx(float x, float y, float rot, float hscale, float vscale,  Rect *rect) const;

protected:
	SpriteData();
	
	Quad		quad;
	float		tx, ty, width, height;
	float		tex_width, tex_height;
	float		hotX, hotY;
	bool		bXFlip, bYFlip, bHSFlip;
};

}
