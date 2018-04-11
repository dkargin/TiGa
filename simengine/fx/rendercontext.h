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

	// Get current viewport size, in pixels
	vec2f getViewportSize() const;

	// This should be moved to 'VertexBatch' system as well. RenderContext should become as thin as possible
	void drawSprite(const SpriteData* sprite,const Pose& p, float width, float height, bool rect=false);
	void drawRect(const Fx::Rect& rect, Fx::FxRawColor color);
	void drawRectSolid(const Fx::Rect& rect, Fx::FxRawColor color);

	/// Should be moved to batch->render as well
	void Render(const SpriteData* spr, float x, float y);
	void RenderEx(const SpriteData* spr, float x, float y, float rot, float hscale=1.0f, float vscale=0.0f);
	void RenderStretch(const SpriteData* spr, float x1, float y1, float x2, float y2);
	void Render4V(const SpriteData* spr, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

	virtual void setClipping(const Rect& rect);
	virtual void disableClipping();


	void Gfx_SetClipping(int x, int y, int w, int h);
	//void Gfx_RenderLine(float x0, float y0, float x1, float y1, FxRawColor rolor = MakeRGB(255, 255, 255), float z = 0);
	//void Gfx_RenderTriple(const Fx::Triple *triple);
	//void Gfx_RenderQuad(const Fx::Quad *quad);
	void renderBatch(const Fx::VertexBatch& batch);
	void Gfx_Clear(FxRawColor color);

	//Fx::Vertex* Gfx_StartBatch(int prim_type, Fx::FxTextureId tex, int blend, int *max_prim);
	//void Gfx_FinishBatch(int nprim);

protected:

	struct TargetBuffer
	{
		int width;
		int height;
		int id;
	};

	struct TextureInfo
	{
		int width;
		int height;
		int id;
		int potw;
		int poth;
	};

	const TextureInfo* getTextureInfo(FxTextureId id);

	bool _PrimsOutsideClipping(const Fx::Vertex *v, const int verts);
	int updateBlendMode(int blend, int curBlend);
	void _BindTexture(FxTextureId id);
	void _ActivateBatch(const Fx::VertexBatch& batch);
	void _render_batch(Fx::VertexBatch& batch, bool bEndScene=false);

	// Viewport width and height
	int scr_width, scr_height;

	// Clipping settings
	float clipX, clipY, clipW, clipH;

	Fx::Vertex* VertArray = nullptr;
	bool bZBuffer = true;
	Fx::Vertex* pVB;	//< vertex buffer is a client-side array in the OpenGL renderer.
	uint16_t* pIB;		//> index buffer is a client-side array in the OpenGL renderer.

	static bool canMergeBatches(const Fx::VertexBatch& a, const Fx::VertexBatch& b);

	Fx::VertexBatch currentBatch;

	//FxTextureId CurTexture = -1;
	// Number of generated primitives
	//int nPrim;
	// Current primitive type
	//int CurPrimType;
	// Current blend mode
	//int CurBlendMode;

	int VERTEX_BUFFER_SIZE = 1024;
};

} // namespace Fx
