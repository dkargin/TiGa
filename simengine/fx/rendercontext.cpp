/*
 * rendercontext.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: vrobot
 */

#include "rendercontext.h"
#include "fxmanager.h"
#include "spritedata.h"

#include "GL/gl.h"
#include <string.h>

#define CALL

namespace Fx
{

RenderContext::RenderContext(FxManager* manager)
{
	this->manager = manager;
}

RenderContext::~RenderContext()
{

}

template <class T> static inline const T Min(const T a, const T b) { return a < b ? a : b; }
template <class T> static inline const T Max(const T a, const T b) { return a > b ? a : b; }

void RenderContext::_ActivateBatch(const Fx::VertexBatch& batch)
{
	if (!currentBatch)
		return;
	currentBatch->primType = batch.primType;

	if(currentBatch->blend != batch.blend)
	{
		currentBatch->blend = updateBlendMode(batch.blend, currentBatch->blend);
	}

	if (currentBatch->texture != batch.texture)
	{
		if(manager)
		{
			if (auto texManager = manager->getTextureManager())
				texManager->bind(batch.texture);
		}
		currentBatch->texture = batch.texture;
	}
}

bool RenderContext::canMergeBatches(const Fx::VertexBatch& a, const Fx::VertexBatch& b)
{
	return a.texture == b.texture && a.blend == b.blend && a.primType == b.primType;
}

bool RenderContext::checkPrimitivesClipping(const Fx::Vertex *v, const int verts)
{
#ifdef FUCK_THIS
	if (bTransforming)
		return false;  // screw it, let the GL do the clipping.
#endif

	const int maxX = clipX + clipW;
	const int maxY = clipY + clipH;

	for (int i = 0; i < verts; i++, v++)
	{
		const int x = v->x;
		const int y = v->y;
		if ((x > clipX) && (x < maxX) && (y > clipY) && (y < maxY))
			return false;
	}
	return true;
}

#ifdef FUCK_THIS
void CALL RenderContext::Gfx_RenderLine(float x1, float y1, float x2, float y2, FxRawColor color, float z)
{
	if (VertArray)
	{
		if(CurPrimType!=HGEPRIM_LINES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_LINES || CurTexture || CurBlendMode!=BLEND_DEFAULT)
		{
			_renderBatchImpl();

			CurPrimType=HGEPRIM_LINES;
			if(CurBlendMode != BLEND_DEFAULT)
				_SetBlendMode(BLEND_DEFAULT);
			_BindTexture(NULL);
		}

		int i=nPrim*HGEPRIM_LINES;
		VertArray[i].x = x1; VertArray[i+1].x = x2;
		VertArray[i].y = y1; VertArray[i+1].y = y2;
		VertArray[i].z     = VertArray[i+1].z = z;
		VertArray[i].col   = VertArray[i+1].col = color;
		VertArray[i].tx    = VertArray[i+1].tx =
		VertArray[i].ty    = VertArray[i+1].ty = 0.0f;

		if (!checkPrimitivesClipping(&VertArray[i], HGEPRIM_LINES))
			nPrim++;
	}
}


void CALL RenderContext::Gfx_RenderTriple(const Fx::Triple* triple)
{
	if (!VertArray)
		return;

	Batch newBatch = {HGEPRIM_TRIPLES, triple->blend, triple->tex};

	const Fx::Vertex* v = triple->v;

	if (checkPrimitivesClipping(v, 3))
	{
		// check for overlap, despite triangle points being outside clipping...
		const int maxX = clipX + clipW;
		const int maxY = clipY + clipH;
		const int leftmost = Min(Min(v[0].x, v[1].x), v[2].x);
		const int rightmost = Max(Max(v[0].x, v[1].x), v[2].x);
		const int topmost = Min(Min(v[0].y, v[1].y), v[2].y);
		const int bottommost = Max(Max(v[0].y, v[1].y), v[2].y);
		if ( ((clipX < leftmost) || (clipX > rightmost)) &&
				 ((maxX < leftmost) || (maxX > rightmost)) &&
				 ((clipY < topmost) || (clipY > bottommost)) &&
				 ((maxY < topmost) || (maxY > bottommost)) )
			return;  // no, this is really totally clipped.
	}

	if (!batchesAreSimilar(newBatch, currentBatch) || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_TRIPLES)
	{
		_renderBatchImpl();
		_ActivateBatch(batch);
	}

	memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(Fx::Vertex)*HGEPRIM_TRIPLES);
	nPrim++;
}

void CALL RenderContext::Gfx_RenderQuad(const Fx::Quad *quad)
{
	if (!VertArray)
		return;

	Batch newBatch = {HGEPRIM_QUADS, triple->blend, triple->tex};

	const Fx::Vertex *v = quad->v;
	if (checkPrimitivesClipping(v, 4))
	{
		// check for overlap, despite quad points being outside clipping...
		const int maxX = clipX + clipW;
		const int maxY = clipY + clipH;
		const int leftmost = Min(Min(Min(v[0].x, v[1].x), v[2].x), v[3].x);
		const int rightmost = Max(Max(Max(v[0].x, v[1].x), v[2].x), v[3].x);
		const int topmost = Min(Min(Min(v[0].y, v[1].y), v[2].y), v[3].y);
		const int bottommost = Max(Max(Max(v[0].y, v[1].y), v[2].y), v[3].y);
		if ( ((clipX < leftmost) || (clipX > rightmost)) &&
				 ((maxX < leftmost) || (maxX > rightmost)) &&
				 ((clipY < topmost) || (clipY > bottommost)) &&
				 ((maxY < topmost) || (maxY > bottommost)) )
			return;  // no, this is really totally clipped.
	}

	if (!batchesAreSimilar(newBatch, currentBatch) ||
			nPrim >= VERTEX_BUFFER_SIZE/HGEPRIM_QUADS)
	{
		_renderBatchImpl();
		_ActivateBatch(newBatch);
	}

	memcpy(&VertArray[nPrim*HGEPRIM_QUADS], quad->v, sizeof(Fx::Vertex)*HGEPRIM_QUADS);
	nPrim++;
}

Fx::Vertex* CALL RenderContext::Gfx_StartBatch(int prim_type, Fx::FxTextureId tex, int blend, int *max_prim)
{
	if(VertArray)
	{
		_renderBatchImpl();

		CurPrimType=prim_type;

		if(CurBlendMode != blend)
			_SetBlendMode(blend);
		_BindTexture(tex);

		*max_prim = VERTEX_BUFFER_SIZE / prim_type;
		return VertArray;
	}
	else return 0;
}

void CALL RenderContext::Gfx_FinishBatch(int nprim)
{
	nPrim = nprim;
}
#endif

void RenderContext::setView( const FxView2 & view, int screenWidth, int screenHeight )
{
	viewPose = view.pose;
	viewScale = view.scale;
	float x = -viewPose.position[0];
	float y = -viewPose.position[1];
	float z = -viewPose.position[2];

	float angle = viewPose.orientation * 180/M_PI;
	// TODO: Should replace this by manual matrix calculation
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-screenWidth/2,screenWidth/2,screenHeight/2,-screenHeight/2,-1,1);
	glScalef(viewScale, viewScale,1);
	glRotatef(-angle,0,0,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(x,y,z);
}

void RenderContext::resetView()
{
	viewPose = math3::Pose2z(0,0,0,0);
	viewScale = 1.0;
}

void CALL RenderContext::clearFrame(FxRawColor color)
{
	GLbitfield flags = GL_COLOR_BUFFER_BIT;
	// TODO:
	//if ( ((pCurTarget) && (pCurTarget->depth)) || bZBuffer )
	//	flags |= GL_DEPTH_BUFFER_BIT;

	const GLfloat a = ((GLfloat) ((color >> 24) & 0xFF)) / 255.0f;
	const GLfloat r = ((GLfloat) ((color >> 16) & 0xFF)) / 255.0f;
	const GLfloat g = ((GLfloat) ((color >>  8) & 0xFF)) / 255.0f;
	const GLfloat b = ((GLfloat) ((color >>  0) & 0xFF)) / 255.0f;
	glClearColor(r, g, b, a);
	glClear(flags);
}

void CALL RenderContext::setClipping(int x, int y, int w, int h)
{
	struct { int X; int Y; int Width; int Height; float MinZ; float MaxZ; } vp;

	/*
	if(!pCurTarget)
	{
		scr_width=pHGE->System_GetStateInt(HGE_SCREENWIDTH);
		scr_height=pHGE->System_GetStateInt(HGE_SCREENHEIGHT);
	}
	else
	{
		scr_width=Texture_GetWidth(pCurTarget->tex);
		scr_height=Texture_GetHeight(pCurTarget->tex);
	}*/

	if(!w) {
		vp.X=0;
		vp.Y=0;
		vp.Width=scr_width;
		vp.Height=scr_height;
	}
	else
	{
		if(x<0) { w+=x; x=0; }
		if(y<0) { h+=y; y=0; }

		if(x+w > scr_width) w=scr_width-x;
		if(y+h > scr_height) h=scr_height-y;

		vp.X=x;
		vp.Y=y;
		vp.Width=w;
		vp.Height=h;
	}

	if ((clipX == vp.X) && (clipY == vp.Y) && (clipW == vp.Width) && (clipH == vp.Height))
		return;   // nothing to do here, don't call into the GL.

	vp.MinZ=0.0f;
	vp.MaxZ=1.0f;

	if (currentBatch)
		_renderBatchImpl(*currentBatch.get());

	clipX = vp.X;
	clipY = vp.Y;
	clipW = vp.Width;
	clipH = vp.Height;
	glScissor(vp.X, (scr_height-vp.Y)-vp.Height, vp.Width, vp.Height);
}

void RenderContext::renderBatch(const Fx::VertexBatch& batch)
{
	if(batch.prims == 0)
		return;
	_renderBatchImpl(batch, false);
}

void RenderContext::_renderBatchImpl(const Fx::VertexBatch& batch, bool bEndScene)
{
	// VertArray is disabled right now
	//if(!VertArray)
	//	return;

	if (batch.prims > 0)
	{
		// texture rectangles range from 0 to size, not 0 to 1.  :/
		float texwmult = 1.0f;
		float texhmult = 1.0f;
#ifdef FUCK_THIS
		if (batch.texture)
		{
			//TODO: _SetTextureFilter();
			const TextureInfo* pTex = getTextureInfo(batch.texture);
			if (pOpenGLDevice->TextureTarget == GL_TEXTURE_RECTANGLE_ARB)
			{
				texwmult = pTex->width;
				texhmult = pTex->height;
			}
			else if ((pTex->potw != 0) && (pTex->poth != 0))
			{
				texwmult = ( ((float)pTex->width) / ((float)pTex->potw) );
				texhmult = ( ((float)pTex->height) / ((float)pTex->poth) );
			}
		}

		const float h = (float) ((pCurTarget) ? pCurTarget->height : nScreenHeight);

		// Wrapping texture coordinates. Should we do it?
		for (int i = 0; i < batch.prims*batch.primType; i++)
		{
			// (0, 0) is the lower left in OpenGL, upper left in D3D.
			VertArray[i].y = h - VertArray[i].y;

			// Z axis is inverted in OpenGL from D3D.
			VertArray[i].z = -VertArray[i].z;

			// (0, 0) is lower left texcoord in OpenGL, upper left in D3D.
			// Also, scale for texture rectangles vs. 2D textures.
			VertArray[i].tx *= texwmult;
			VertArray[i].ty = (1.0f - VertArray[i].ty) * texhmult;

			// Colors are RGBA in OpenGL, ARGB in Direct3D.
			const FxRawColor color = VertArray[i].col;
			uint8_t *col = (uint8_t *) &VertArray[i].col;
			const uint8_t a = ((color >> 24) & 0xFF);
			const uint8_t r = ((color >> 16) & 0xFF);
			const uint8_t g = ((color >>  8) & 0xFF);
			const uint8_t b = ((color >>  0) & 0xFF);
			col[0] = r;
			col[1] = g;
			col[2] = b;
			col[3] = a;
		}

#endif

		switch(batch.primType)
		{
			case VertexBatch::PRIM_QUADS:
				glDrawElements(GL_TRIANGLES, batch.prims * 6, GL_UNSIGNED_SHORT, pIB);
				#if DEBUG_VERTICES
				for (int i = 0; i < nPrim*6; i+=3)
				{
					printf("QUAD'S TRIANGLE:\n");
					print_vertex(&pVB[pIB[i+0]]);
					print_vertex(&pVB[pIB[i+1]]);
					print_vertex(&pVB[pIB[i+2]]);
				}
				printf("DONE.\n");
				#endif
				break;

			case VertexBatch::PRIM_TRIPLES:
				glDrawArrays(GL_TRIANGLES, 0, batch.prims * 3);
				break;

			case VertexBatch::PRIM_LINES:
				glDrawArrays(GL_LINES, 0, batch.prims * 2);
				break;
		}
		//batch.prims = 0;
	}

	if(bEndScene)
		VertArray = 0;
	else
		VertArray = pVB;
}

int RenderContext::updateBlendMode(int blend, int curBlend)
{
	if((blend & BLEND_ALPHABLEND) != (curBlend & BLEND_ALPHABLEND))
	{
		if(blend & BLEND_ALPHABLEND)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}

	if((blend & BLEND_ZWRITE) != (curBlend & BLEND_ZWRITE))
	{
		if(blend & BLEND_ZWRITE)
			glDepthMask(GL_TRUE);
		else
			glDepthMask(GL_FALSE);
	}

	if((blend & BLEND_COLORADD) != (curBlend & BLEND_COLORADD))
	{
		if(blend & BLEND_COLORADD)
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		else
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	return blend;
}

void RenderContext::setClipping(const Rect& rect)
{
	// TODO: Implement
}

void RenderContext::disableClipping()
{
	// TODO: Implement
}

// generate OBB and render sprite in it
void drawSprite(RenderContext* rc, const SpriteData& sprite,
			const Pose &p, float width, float height)
{
	VertexBatch batch(VertexBatch::PRIM_QUADS);

	auto trect = sprite.getTextureRect();
	auto color = sprite.getColor();

	batch += {
		Vertex::make2c(p.coords(-width, height), color, vec2f(trect.x1, trect.y1)),
		Vertex::make2c(p.coords( width, height), color, vec2f(trect.x2, trect.y1)),
		Vertex::make2c(p.coords( width,-height), color, vec2f(trect.x2, trect.y2)),
		Vertex::make2c(p.coords(-width,-height), color, vec2f(trect.x1, trect.y2)),
	};

	batch.blend = sprite.getBlendMode();
	batch.texture = sprite.getTexture();
	rc->renderBatch(batch);
}

void drawSprite(RenderContext* rc, const SpriteData& sprite, const Pose &p)
{
	float width = sprite.getWidth();
	float height = sprite.getHeight();
	drawSprite(rc, sprite, p, width, height);
}

void drawSprite(RenderContext* rc, const SpriteData& sprite, float x, float y)
{
	VertexBatch batch(VertexBatch::PRIM_QUADS);

	auto trect = sprite.getTextureRect();
	auto color = sprite.getColor();

	float x1 = x + sprite.getWidth();
	float y1 = y + sprite.getHeight();
	batch += {
		Vertex::make2c( x, y, color, trect.x1, trect.y1),
		Vertex::make2c(x1, y, color, trect.x2, trect.y1),
		Vertex::make2c(x1,y1, color, trect.x2, trect.y2),
		Vertex::make2c( x,y1, color, trect.x1, trect.y2),
	};

	batch.blend = sprite.getBlendMode();
	batch.texture = sprite.getTexture();
	rc->renderBatch(batch);
}

void drawRect(RenderContext* rc, const Fx::Rect& rect, Fx::FxRawColor color)
{
	VertexBatch batch(VertexBatch::PRIM_LINES);

	float x1 = rect.x1;
	float y1 = rect.y1;
	float x2 = rect.x2;
	float y2 = rect.y2;

	batch += {
		Vertex::make2c(x1, y1, color, 0.0, 0.0),
		Vertex::make2c(x2, y1, color, 1.0, 0.0),

		Vertex::make2c(x2, y1, color, 1.0, 0.0),
		Vertex::make2c(x2, y2, color, 1.0, 1.0),

		Vertex::make2c(x2, y2, color, 1.0, 1.0),
		Vertex::make2c(x1, y2, color, 0.0, 1.0),

		Vertex::make2c(x1, y2, color, 0.0, 1.0),
		Vertex::make2c(x1, y1, color, 0.0, 0.0),
	};

	//batch.blend = sprite.getBlendMode();
	batch.texture = 0;
	rc->renderBatch(batch);
}

void drawRectSolid(RenderContext* rc, const Fx::Rect& rect, Fx::FxRawColor color)
{
	VertexBatch batch(VertexBatch::PRIM_QUADS);

	float x1 = rect.x1;
	float y1 = rect.y1;
	float x2 = rect.x2;
	float y2 = rect.y2;

	batch += {
		Vertex::make2c(x1, y1, color, 0.0, 0.0),
		Vertex::make2c(x2, y1, color, 1.0, 0.0),
		Vertex::make2c(x2, y2, color, 1.0, 1.0),
		Vertex::make2c(x1, y2, color, 0.0, 1.0),
	};

	//batch.blend = sprite.getBlendMode();
	batch.texture = 0;
	rc->renderBatch(batch);
}


} // namespace fx


