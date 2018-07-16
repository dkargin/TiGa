#pragma once

#include <memory>	// for shared_ptr/unique_ptr
#include "basetypes.h"


namespace Fx
{

class SpriteData;
class FxManager;

/**
 * RenderContext class
 * Wraps rendering pass
 * Any object rendering should receive pointer to instance of RenderContext
 */
class RenderContext
{
public:
	RenderContext(FxManager* manager);
	virtual ~RenderContext();

	// Get current viewport size, in pixels
	vec2f getViewportSize() const;

	/// Should be moved to batch->render as well
	/*
	void Render(const SpriteData* spr, float x, float y);
	void RenderEx(const SpriteData* spr, float x, float y, float rot, float hscale=1.0f, float vscale=0.0f);
	void RenderStretch(const SpriteData* spr, float x1, float y1, float x2, float y2);
	void Render4V(const SpriteData* spr, float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);
	void Gfx_RenderLine(float x0, float y0, float x1, float y1, FxRawColor rolor = MakeRGB(255, 255, 255), float z = 0);
	void Gfx_RenderTriple(const Fx::Triple *triple);
	void Gfx_RenderQuad(const Fx::Quad *quad);
	*/

	void setClipping(const Rect& rect);
	void setClipping(int x, int y, int w, int h);
	void disableClipping();

	void renderBatch(const Fx::VertexBatch& batch);
	void clearFrame(FxRawColor color);

protected:
	struct TargetBuffer
	{
		int width;
		int height;
		int id;
	};

	// TODO: To be moved to texture manager class
	struct TextureInfo
	{
		int width;
		int height;
		int id;
		int potw;
		int poth;
	};

	const TextureInfo* getTextureInfo(FxTextureId id);

	bool checkPrimitivesClipping(const Fx::Vertex *v, const int verts);
	int updateBlendMode(int blend, int curBlend);

	void _ActivateBatch(const Fx::VertexBatch& batch);
	void _renderBatchImpl(Fx::VertexBatch& batch, bool bEndScene=false);

	// Viewport width and height
	int scr_width = 0, scr_height = 0;

	// Clipping settings
	float clipX, clipY, clipW, clipH;

	Fx::Vertex* VertArray = nullptr;
	bool bZBuffer = true;
	Fx::Vertex* pVB = nullptr; //< vertex buffer is a client-side array in the OpenGL renderer.
	uint16_t* pIB = nullptr; //> index buffer is a client-side array in the OpenGL renderer.

	static bool canMergeBatches(const Fx::VertexBatch& a, const Fx::VertexBatch& b);

	std::unique_ptr<Fx::VertexBatch> currentBatch;

	int VERTEX_BUFFER_SIZE = 1024;

	FxManager* manager;
};

void drawSprite(RenderContext* rc, const SpriteData& sprite, const Pose &p);
void drawSprite(RenderContext* rc, const SpriteData& sprite, const Pose &p, float width, float height);
void drawSprite(RenderContext* rc, const SpriteData& sprite, float x, float y);
void drawRect(RenderContext* rc, const Fx::Rect& rect, Fx::FxRawColor color);
void drawRectSolid(RenderContext* rc, const Fx::Rect& rect, Fx::FxRawColor color);
} // namespace Fx
