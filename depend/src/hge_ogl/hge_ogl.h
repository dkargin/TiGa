/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Common core implementation header
*/


#pragma once

//#include "..\core\hge_plugin.h"
#include "hge.h"

#include <windows.h>

#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "SOIL\soil.h"

#define VERTEX_BUFFER_SIZE 4000

struct CRenderTargetList
{
	int					width;
	int					height;
	GLuint				pTex;
	GLuint				pDepth;
	CRenderTargetList*	next;
};

struct CTextureList
{
	HTEXTURE			tex;
	int					width;
	int					height;
	CTextureList*		next;
};

/*
** HGE Plugin implementation
*/
class HGE_OGL : public HGE_Plugin
{
	public:
		virtual	void		CALL	Release();

		virtual bool		CALL	System_Initiate();

		virtual bool		CALL	Gfx_BeginScene(HTARGET target=0);
		virtual void		CALL	Gfx_EndScene();
		virtual void		CALL	Gfx_Clear(DWORD color);
		virtual void		CALL	Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color=0xFFFFFFFF, float z=0.5f);
		virtual void		CALL	Gfx_RenderTriple(const hgeTriple *triple);
		virtual void		CALL	Gfx_RenderQuad(const hgeQuad *quad);
		virtual hgeVertex*	CALL	Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim);
		virtual void		CALL	Gfx_FinishBatch(int nprim);
		virtual void		CALL	Gfx_SetClipping(int x=0, int y=0, int w=0, int h=0);
		virtual void		CALL	Gfx_SetTransform(float x=0, float y=0, float dx=0, float dy=0, float rot=0, float hscale=0, float vscale=0); 

		virtual HSHADER		CALL	Shader_Create(const char *filename);
		virtual void		CALL	Shader_Free(HSHADER shader);
		virtual void		CALL	Gfx_SetShader(HSHADER shader);

		virtual HTARGET		CALL	Target_Create(int width, int height, bool zbuffer);
		virtual void		CALL	Target_Free(HTARGET target);
		virtual HTEXTURE	CALL	Target_GetTexture(HTARGET target);

		virtual HTEXTURE	CALL	Texture_Create(int width, int height);
		virtual HTEXTURE	CALL	Texture_Load(const char *filename, DWORD size=0, bool bMipmap=false);
		virtual void		CALL	Texture_Free(HTEXTURE tex);
		virtual int			CALL	Texture_GetWidth(HTEXTURE tex, bool bOriginal=false);
		virtual int			CALL	Texture_GetHeight(HTEXTURE tex, bool bOriginal=false);
		virtual DWORD*		CALL	Texture_Lock(HTEXTURE tex, bool bReadOnly=true, int left=0, int top=0, int width=0, int height=0);
		virtual void		CALL	Texture_Unlock(HTEXTURE tex);

		static  HGE_OGL *			_Interface_Get(HGE *lpHGE);

	private:
		HGE_OGL(HGE *lpHGE);

		bool _GfxInit();
		void _GfxDone();
		bool _GfxRestore();
		void _AdjustWindow();
		void _Resize(int width, int height);
		bool _init_lost();
		void _render_batch(bool bEndScene = false);
		void _SetBlendMode(int blend);
		void _SetProjectionMatrix(int width, int height);

		void _SplitColor(DWORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a);
		DWORD _FlipColor(DWORD old);
		int _FixedTextureSize(int size);

		// Graphics
		HDC					hDC;
		HGLRC				hRC;
		GLuint				vboID; // vertex buffer
		GLuint				iboID; // index buffer
		GLuint				pScreenSurf;
		GLuint				pScreenDepth;

		RECT				rectW;
		LONG				styleW;
		RECT				rectFS;
		LONG				styleFS;

		CRenderTargetList*	pTargets;
		CRenderTargetList*	pCurTarget;

		CTextureList*		textures;
		hgeVertex*			VertArray;

		int					nPrim;
		int					CurPrimType;
		int					CurBlendMode;
		HTEXTURE			CurTexture;
		HSHADER				CurShader;

		int					nScreenWidth;
		int					nScreenHeight;
};

extern HGE_OGL * g_lpHGE;
