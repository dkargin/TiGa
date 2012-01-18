#include "hge_ogl.h"

#include <memory.h>

int			g_nRef = 0;
HGE_OGL *	g_lpHGE = 0;

#pragma warning(disable:4996)
/*
HGE * CALL Plugin_Initialize(HGE *lpHGE)
{
	return (HGE *) HGE_OGL::_Interface_Get(lpHGE);
}*/

HGE_OGL * HGE_OGL::_Interface_Get(HGE *lpHGE)
{
	if (!g_lpHGE) g_lpHGE = new HGE_OGL(lpHGE);
	g_nRef++;

	return g_lpHGE;
}

HGE_OGL::HGE_OGL(HGE *lpHGE)
{
	m_lpHGE = lpHGE;
}

void CALL HGE_OGL::Release()
{
	g_nRef--;

	if (g_nRef == 0)
	{
		delete g_lpHGE;
		g_lpHGE = 0;
	}
}

bool CALL HGE_OGL::System_Initiate()
{
	return _GfxInit();
}

void CALL HGE_OGL::Gfx_Clear(DWORD color)
{
	BYTE r, g, b, a;
	_SplitColor(color, r, g, b, a);

	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void CALL HGE_OGL::Gfx_SetClipping(int x, int y, int w, int h)
{
	_render_batch();

	if (x == 0 && y == 0 && w == 0 && h == 0)
	{
		glScissor(0, 0, m_lpHGE->System_GetState(HGE_SCREENWIDTH), m_lpHGE->System_GetState(HGE_SCREENHEIGHT));
		glDisable(GL_SCISSOR_TEST);
	}
	else
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(x, y, w, h);
	}
}

void CALL HGE_OGL::Gfx_SetTransform(float x, float y, float dx, float dy, float rot, float hscale, float vscale)
{
	if (vscale == 0.0f)
	{
		// reset
		glLoadIdentity();
	}
	else
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(-x, -y, 0.0f);
		glScalef(hscale, vscale, 1.0f);
		glRotatef(rot, 0.0f, 0.0f, 1.0f);
	}

	_render_batch();
}

bool CALL HGE_OGL::Gfx_BeginScene(HTARGET targ)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if (targ == 0)
	{
		int scr_width = m_lpHGE->System_GetState(HGE_SCREENWIDTH);
		int scr_height = m_lpHGE->System_GetState(HGE_SCREENHEIGHT);
		glOrtho(0, (double) scr_width, (double) scr_height, 0, -1, 1);
		glViewport(0, 0, scr_width, scr_height);
	}
	else	
	{
		/* FRAME BUFFER IMPLEMENTATION HERE
		if (target)
		{
			target->pTex->GetSurfaceLevel(0, &pSurf);
			pDepth = target->pDepth;
		}
		else
		{
			pSurf = pScreenSurf;
			pDepth = pScreenDepth;
		}
		*/
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);

	glDisable(GL_LIGHTING);
	glDisable(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);

	CurBlendMode = 0;
	_SetBlendMode(BLEND_DEFAULT);

	glDepthFunc(GL_LEQUAL);

	if (VertArray) delete [] VertArray;
	VertArray = new hgeVertex[(VERTEX_BUFFER_SIZE * 6 / 4 * sizeof(WORD))];

	return true;
}

HSHADER CALL HGE_OGL::Shader_Create(const char *filename)
{
	// not yet implemented
	return NULL;
}

void CALL HGE_OGL::Gfx_SetShader(HSHADER shader)
{
	// not yet implemented
}

void CALL HGE_OGL::Shader_Free(HSHADER shader)
{
	// not yet implemented
}

void CALL HGE_OGL::Gfx_EndScene()
{
	_render_batch(true);

	glPopAttrib();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	SwapBuffers(hDC);
}

void CALL HGE_OGL::Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color, float z)
{
	if (VertArray)
	{
		if (CurPrimType != HGEPRIM_LINES || nPrim >= VERTEX_BUFFER_SIZE / HGEPRIM_LINES || CurTexture || CurBlendMode != BLEND_DEFAULT)
		{
			_render_batch();
			CurPrimType = HGEPRIM_LINES;

			if (CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);

			if (CurTexture)
			{
				glBindTexture(GL_TEXTURE_2D, NULL);
				CurTexture = 0; 
			}
		}

		int i = nPrim * HGEPRIM_LINES;

		VertArray[i].x = x1; VertArray[i + 1].x = x2;
		VertArray[i].y = y1; VertArray[i + 1].y = y2;
		VertArray[i].z     = VertArray[i + 1].z = z;
		VertArray[i].col   = VertArray[i + 1].col = color;
		VertArray[i].tx    = VertArray[i + 1].tx = 0.0f;
		VertArray[i].ty    = VertArray[i + 1].ty = 0.0f;

		nPrim++;
	}
}

void CALL HGE_OGL::Gfx_RenderTriple(const hgeTriple *triple)
{
	if(VertArray)
	{
		if(CurPrimType != HGEPRIM_TRIPLES || nPrim >= VERTEX_BUFFER_SIZE / HGEPRIM_TRIPLES || CurTexture != triple->tex || CurBlendMode != triple->blend)
		{
			_render_batch();
			CurPrimType = HGEPRIM_TRIPLES;

			if(CurBlendMode != triple->blend) _SetBlendMode(triple->blend);

			if (triple->tex != CurTexture)
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint) triple->tex);
				CurTexture = triple->tex;
			}
		}

		memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(hgeVertex) * HGEPRIM_TRIPLES);
		nPrim++;
	}
}

void CALL HGE_OGL::Gfx_RenderQuad(const hgeQuad *quad)
{
	if (VertArray)
	{
		if (CurPrimType != HGEPRIM_QUADS || nPrim >= VERTEX_BUFFER_SIZE / HGEPRIM_QUADS || CurTexture != quad->tex || CurBlendMode != quad->blend)
		{
			_render_batch();
			CurPrimType = HGEPRIM_QUADS;

			if (CurBlendMode != quad->blend) _SetBlendMode(quad->blend);

			if (quad->tex != CurTexture)
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint) quad->tex);
				CurTexture = quad->tex;
			}
		}

		memcpy(&VertArray[nPrim * HGEPRIM_QUADS], quad->v, sizeof(hgeVertex) * HGEPRIM_QUADS);
		nPrim++;
	}
}

hgeVertex* CALL HGE_OGL::Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim)
{
	if (VertArray)
	{
		_render_batch();
		CurPrimType = prim_type;

		if (CurBlendMode != blend) _SetBlendMode(blend);

		if (tex != CurTexture)
		{
			glBindTexture(GL_TEXTURE_2D, (GLuint) tex);
			CurTexture = tex;
		}

		*max_prim = VERTEX_BUFFER_SIZE / prim_type;
		return VertArray;
	}
	else
	{
		return 0;
	}
}

void CALL HGE_OGL::Gfx_FinishBatch(int nprim)
{
	nPrim = nprim;
}

HTARGET CALL HGE_OGL::Target_Create(int width, int height, bool zbuffer)
{
	CRenderTargetList *pTarget;

	pTarget = new CRenderTargetList;

	pTarget->pTex = Texture_Create(width, height);
	pTarget->width = _FixedTextureSize(width);
	pTarget->height = _FixedTextureSize(height);
	pTarget->pDepth = 0;

	if (zbuffer)
	{
		// do some crap here to support z-buffers >.>
	}

	pTarget->next = pTargets;
	pTargets = pTarget;

	return (HTARGET)pTarget;
}

void CALL HGE_OGL::Target_Free(HTARGET target)
{
	CRenderTargetList *pTarget = pTargets, *pPrevTarget = NULL;

	while (pTarget)
	{
		if((CRenderTargetList *)target == pTarget)
		{
			if(pPrevTarget)
				pPrevTarget->next = pTarget->next;
			else
				pTargets = pTarget->next;

			if (pTarget->pTex) glDeleteTextures(1, &pTarget->pTex);

			delete pTarget;
			return;
		}

		pPrevTarget = pTarget;
		pTarget = pTarget->next;
	}
}

HTEXTURE CALL HGE_OGL::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ = (CRenderTargetList *) target;
	if(target) return (HTEXTURE) targ->pTex;
	else return 0;
}

HTEXTURE CALL HGE_OGL::Texture_Create(int width, int height)
{
	GLuint  pTex;
	int fixed_width = _FixedTextureSize(width), fixed_height = _FixedTextureSize(height);
	unsigned int *data; // used to create memory for texture area

	data = (unsigned int *) new GLuint[((fixed_width * fixed_height) * 4 * sizeof(unsigned int))];
	memset(data, 0, ((fixed_width * fixed_height) * 4 * sizeof(unsigned int)));

	glGenTextures(1, &pTex);
	glBindTexture(GL_TEXTURE_2D, pTex);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, fixed_width, fixed_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	delete [] data;

	return (HTEXTURE) pTex;
}

HTEXTURE CALL HGE_OGL::Texture_Load(const char *filename, DWORD size, bool bMipmap)
{
	int width, height;
	void *data;
	DWORD _size;
	CTextureList *texItem;

	if (size)
	{
		data = (void *) filename;
		_size = size;
	}
	else
	{
		data = m_lpHGE->Resource_Load(filename, &_size);
		if (!data) return NULL;
	}

	int uiImageWidth, uiImageHeight, uiImageChannels;
	GLuint pTex;
	GLint prevTex;

	unsigned char *PixelData = SOIL_load_image_from_memory((unsigned char *) data, _size, &uiImageWidth, &uiImageHeight, &uiImageChannels, SOIL_LOAD_RGBA);

	if (PixelData != NULL)
	{
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);

		pTex = Texture_Create(uiImageWidth, uiImageHeight);

		glBindTexture(GL_TEXTURE_2D, pTex);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, uiImageWidth, uiImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, PixelData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		SOIL_free_image_data(PixelData);

		width = uiImageWidth;
		height = uiImageHeight;

		glBindTexture(GL_TEXTURE_2D, prevTex);
	}

	if (!size) m_lpHGE->Resource_Free(data);
	
	texItem = new CTextureList;
	texItem->tex =   (HTEXTURE) pTex;
	texItem->width =  width;
	texItem->height = height;
	texItem->next =   textures;
	textures = texItem;

	return (HTEXTURE) pTex;
}

void CALL HGE_OGL::Texture_Free(HTEXTURE tex)
{
	GLuint pTex = (GLuint) tex;
	CTextureList *texItem = textures, *texPrev = 0;

	while (texItem)
	{
		if (texItem->tex == tex)
		{
			if (texPrev)
			{
				texPrev->next = texItem->next;
			}
			else
			{
				textures = texItem->next;
			}

			delete texItem;
			break;
		}

		texPrev = texItem;
		texItem = texItem->next;
	}

	if (pTex != NULL) glDeleteTextures(1, &pTex);
}

int CALL HGE_OGL::Texture_GetWidth(HTEXTURE tex, bool bOriginal)
{
	GLuint pTex = (GLuint) tex;
	CTextureList *texItem = textures;
	int width = 0;

	if (bOriginal)
	{
		while (texItem && width == 0)
		{
			if (texItem->tex == tex)
			{
				width = texItem->width;
			}

			texItem = texItem->next;
		}
	}
	else
	{
		GLfloat fWidth;
		GLint curTex;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);

		glBindTexture(GL_TEXTURE_2D, pTex);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &fWidth);

		glBindTexture(GL_TEXTURE_2D, curTex);

		width = (int) fWidth;
	}

	return width;
}


int CALL HGE_OGL::Texture_GetHeight(HTEXTURE tex, bool bOriginal)
{
	GLuint pTex = (GLuint) tex;
	CTextureList *texItem = textures;
	int height = 0;

	if (bOriginal)
	{
		while (texItem && height == 0)
		{
			if (texItem->tex == tex)
			{
				height = texItem->height;
			}

			texItem = texItem->next;
		}
	}
	else
	{
		GLfloat fHeight;
		GLint curTex;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);

		glBindTexture(GL_TEXTURE_2D, pTex);
		glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &fHeight);

		glBindTexture(GL_TEXTURE_2D, curTex);

		height = (int) fHeight;
	}

	return height;
}


DWORD * CALL HGE_OGL::Texture_Lock(HTEXTURE tex, bool bReadOnly, int left, int top, int width, int height)
{
	// not yet implemented
	return NULL;
}


void CALL HGE_OGL::Texture_Unlock(HTEXTURE tex)
{
	// not yet implemented
}

//////// Implementation ////////

void HGE_OGL::_render_batch(bool bEndScene)
{
	if (VertArray)
	{
		if (nPrim)
		{
			for (int t = 0; t < nPrim * CurPrimType; t++)
			{
				VertArray[t].col = _FlipColor(VertArray[t].col);
			}

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &VertArray[0]);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &VertArray[0].col);
			glTexCoordPointer(2, GL_FLOAT, sizeof(hgeVertex), &VertArray[0].tx);

			switch(CurPrimType)
			{
				case HGEPRIM_QUADS:
				{
					glDrawArrays(GL_QUADS, 0, nPrim * HGEPRIM_QUADS);
				} break;

				case HGEPRIM_TRIPLES:
				{
					glDrawArrays(GL_TRIANGLES, 0, nPrim * HGEPRIM_TRIPLES);
				} break;

				case HGEPRIM_LINES:
				{
					glDrawArrays(GL_LINES, 0, nPrim * HGEPRIM_LINES);
				} break;
			}

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			nPrim = 0;
		}

		if (bEndScene)
		{
			if (VertArray) delete [] VertArray;
			VertArray = 0;
		}
		else
		{
			memset(VertArray, 0, (sizeof(hgeVertex) * (VERTEX_BUFFER_SIZE * 6 / 4 * sizeof(WORD))));
		}
	}
}

void HGE_OGL::_SetBlendMode(int blend)
{
	if ((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
	{
		if (blend & BLEND_ALPHABLEND)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
	}

	if ((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
	{
		if (blend & BLEND_COLORADD)
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		}
		else
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}
	}

	if ((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
	{
		if (blend & BLEND_ZWRITE)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}
	}

	CurBlendMode = blend;
}

void HGE_OGL::_SetProjectionMatrix(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool HGE_OGL::_GfxInit()
{
	static PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
	GLuint gluiPixelFormat;

	hDC = GetDC((HWND) m_lpHGE->System_GetState(HGE_HWND));

	if (!(gluiPixelFormat = ChoosePixelFormat(hDC, &pfd))) return false;
	if (!(SetPixelFormat(hDC, gluiPixelFormat, &pfd))) return false;
	if (!(hRC = wglCreateContext(hDC))) return false;
	if (!(wglMakeCurrent(hDC, hRC))) return false;

	m_lpHGE->System_Log("OGL Driver: %s (hrmmm)", (char *) glGetString(GL_RENDERER));
	m_lpHGE->System_Log("Description: %s", (char *) glGetString(GL_VENDOR));
	m_lpHGE->System_Log("Version: %s", (char *) glGetString(GL_VERSION));
	m_lpHGE->System_Log("Mode: %d x %d\n", System_GetState(HGE_SCREENWIDTH), System_GetState(HGE_SCREENHEIGHT));

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	VertArray = 0;
	textures = 0;
	
	if(!_init_lost()) return false;
	Gfx_Clear(0);

	return true;
}

void HGE_OGL::_AdjustWindow()
{
	RECT *rc;
	LONG style;
	HWND hwnd = (HWND) m_lpHGE->System_GetState(HGE_HWND);
	bool bWindowed = m_lpHGE->System_GetState(HGE_WINDOWED);

	if (bWindowed) {rc=&rectW; style=styleW; }
	else  {rc=&rectFS; style=styleFS; }
	SetWindowLong(hwnd, GWL_STYLE, style);

	style=GetWindowLong(hwnd, GWL_EXSTYLE);

	if(bWindowed)
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
	    SetWindowPos(hwnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
	else
	{
		SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
	    SetWindowPos(hwnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
	}
}

void HGE_OGL::_Resize(int width, int height)
{
	HWND hwndParent = (HWND) m_lpHGE->System_GetState(HGE_HWNDPARENT);

	if (hwndParent)
	{
		nScreenWidth=width;
		nScreenHeight=height;

		_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		_GfxRestore();
	}
}

void HGE_OGL::_GfxDone()
{
	CRenderTargetList *target=pTargets, *next_target;
	
	while (textures)	Texture_Free(textures->tex);

	if(pScreenSurf) { glDeleteTextures(1, &pScreenSurf); pScreenSurf=0; }
	if(pScreenDepth) { glDeleteTextures(1, &pScreenDepth); pScreenDepth=0; }

	while(target)
	{
		if (target->pTex) glDeleteTextures(1, &target->pTex);

		next_target=target->next;
		delete target;
		target=next_target;
	}

	pTargets=0;
}


bool HGE_OGL::_GfxRestore()
{
	CRenderTargetList *target=pTargets;

	if(pScreenSurf) { glDeleteTextures(1, &pScreenSurf); pScreenSurf=0; }
	if(pScreenDepth) { glDeleteTextures(1, &pScreenDepth); pScreenDepth=0; }

	while(target)
	{
		if (target->pTex) glDeleteTextures(1, &target->pTex);
		target=target->next;
	}

	if(!_init_lost()) return false;

	//if (procGfxRestoreFunc) return procGfxRestoreFunc();

	return true;
}


bool HGE_OGL::_init_lost()
{
	// Store render target
	pScreenSurf = 0;
	pScreenDepth = 0;
	nPrim = 0;

	CurPrimType = HGEPRIM_QUADS;
	CurBlendMode = BLEND_DEFAULT;
	CurTexture = NULL;

	if (VertArray) delete [] VertArray;
	VertArray = new hgeVertex[(VERTEX_BUFFER_SIZE * 6 / 4 * sizeof(WORD))];

	return true;
}

void HGE_OGL::_SplitColor(DWORD color, BYTE &r, BYTE &g, BYTE &b, BYTE &a)
{
	if (color > 0xFFFFFF)
	{
		r = (color & 0xFF000000) >> 24;
		g = (color & 0x00FF0000) >> 16;
		b = (color & 0x0000FF00) >> 8;
		a = (color & 0x000000FF);
	}
	else
	{
		r = (color & 0xFF0000) >> 16;
		g = (color & 0x00FF00) >> 8;
		b = (color & 0x0000FF);
		a = 0xFF;
	}
}

DWORD HGE_OGL::_FlipColor(DWORD old)
{
	return (old & 0xFF000000) | (old & 0x0000FF00) | ((old & 0x00FF0000) >> 16) | ((old & 0x000000FF) << 16);
}

int HGE_OGL::_FixedTextureSize(int size)
{
	int nsize = 1;

	while (nsize < size)
	{
		nsize *= 2;
	}

	return nsize;
}
