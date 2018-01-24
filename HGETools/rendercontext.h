#pragma once

#include "basetypes.h"
namespace Fx
{

class SpriteData;

/**
 * RenderContext class
 * Wraps rendering pass
 * Any object rendering should receive pointer to instance of RenderContext
 */
class RenderContext
{
public:
	virtual ~RenderContext();

	void drawSprite(const SpriteData* sprite,const Pose& p, float width, float height, bool rect=false);
	void drawRect(const Fx::Rect& rect, Fx::FxRawColor color);
	void drawRectSolid(const Fx::Rect& rect, Fx::FxRawColor color);
	virtual void setClipping(const Rect& rect);
	virtual void disableClipping();

	/// Should be moved to batch->render
	void		Render(const SpriteData* spr, float x, float y);
	void		RenderEx(const SpriteData* spr, float x, float y, float rot, float hscale=1.0f, float vscale=0.0f);
	void		RenderStretch(const SpriteData* spr, float x1, float y1, float x2, float y2);
	void		Render4V(const SpriteData* spr, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
};

} // namespace Fx
