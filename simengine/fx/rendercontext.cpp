/*
 * rendercontext.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: vrobot
 */

#include "rendercontext.h"
#include "GL/gl.h"

#define CALL

namespace Fx
{

bool RenderContext::_PrimsOutsideClipping(const Fx::Vertex *v, const int verts)
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

void CALL RenderContext::Gfx_RenderLine(float x1, float y1, float x2, float y2, FxRawColor color, float z)
{
	if (VertArray)
	{
		if(CurPrimType!=HGEPRIM_LINES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_LINES || CurTexture || CurBlendMode!=BLEND_DEFAULT)
		{
			_render_batch();

			CurPrimType=HGEPRIM_LINES;
			if(CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);
			_BindTexture(NULL);
		}

		int i=nPrim*HGEPRIM_LINES;
		VertArray[i].x = x1; VertArray[i+1].x = x2;
		VertArray[i].y = y1; VertArray[i+1].y = y2;
		VertArray[i].z     = VertArray[i+1].z = z;
		VertArray[i].col   = VertArray[i+1].col = color;
		VertArray[i].tx    = VertArray[i+1].tx =
		VertArray[i].ty    = VertArray[i+1].ty = 0.0f;

		if (!_PrimsOutsideClipping(&VertArray[i], HGEPRIM_LINES))
			nPrim++;
	}
}

template <class T> static inline const T Min(const T a, const T b) { return a < b ? a : b; }
template <class T> static inline const T Max(const T a, const T b) { return a > b ? a : b; }

void CALL RenderContext::Gfx_RenderTriple(const Fx::Triple *triple)
{
	if (!VertArray)
		return;

	const Fx::Vertex* v = triple->v;

	if (_PrimsOutsideClipping(v, 3))
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

	if(CurPrimType!=HGEPRIM_TRIPLES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_TRIPLES || CurTexture!=triple->tex || CurBlendMode!=triple->blend)
	{
		_render_batch();

		CurPrimType=HGEPRIM_TRIPLES;
		if(CurBlendMode != triple->blend) _SetBlendMode(triple->blend);
		_BindTexture((gltexture *) triple->tex);
	}

	memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(Fx::Vertex)*HGEPRIM_TRIPLES);
	nPrim++;

}

void CALL RenderContext::Gfx_RenderQuad(const Fx::Quad *quad)
{
	if (VertArray)
	{
		const Fx::Vertex *v = quad->v;
		if (_PrimsOutsideClipping(v, 4))
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

		if(CurPrimType!=HGEPRIM_QUADS || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_QUADS || CurTexture!=quad->tex || CurBlendMode!=quad->blend)
		{
			_render_batch();

			CurPrimType=HGEPRIM_QUADS;
			if(CurBlendMode != quad->blend)
				_SetBlendMode(quad->blend);
			_BindTexture((gltexture *) quad->tex);
		}

		memcpy(&VertArray[nPrim*HGEPRIM_QUADS], quad->v, sizeof(Fx::Vertex)*HGEPRIM_QUADS);
		nPrim++;
	}
}

Fx::Vertex* CALL RenderContext::Gfx_StartBatch(int prim_type, Fx::FxTextureId tex, int blend, int *max_prim)
{
	if(VertArray)
	{
		_render_batch();

		CurPrimType=prim_type;

		if(CurBlendMode != blend)
			_SetBlendMode(blend);
		_BindTexture((gltexture *) tex);
		*max_prim=VERTEX_BUFFER_SIZE / prim_type;
		return VertArray;
	}
	else return 0;
}

void CALL RenderContext::Gfx_FinishBatch(int nprim)
{
	nPrim = nprim;
}


void CALL RenderContext::Gfx_Clear(FxRawColor color)
{
	GLbitfield flags = GL_COLOR_BUFFER_BIT;
	if ( ((pCurTarget) && (pCurTarget->depth)) || bZBuffer )
		flags |= GL_DEPTH_BUFFER_BIT;

	const GLfloat a = ((GLfloat) ((color >> 24) & 0xFF)) / 255.0f;
	const GLfloat r = ((GLfloat) ((color >> 16) & 0xFF)) / 255.0f;
	const GLfloat g = ((GLfloat) ((color >>  8) & 0xFF)) / 255.0f;
	const GLfloat b = ((GLfloat) ((color >>  0) & 0xFF)) / 255.0f;
	glClearColor(r, g, b, a);
	glClear(flags);
}

void CALL RenderContext::Gfx_SetClipping(int x, int y, int w, int h)
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

	_render_batch();

	clipX = vp.X;
	clipY = vp.Y;
	clipW = vp.Width;
	clipH = vp.Height;
	glScissor(vp.X, (scr_height-vp.Y)-vp.Height, vp.Width, vp.Height);
}


}


