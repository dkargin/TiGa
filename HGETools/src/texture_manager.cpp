/*
 * texture_manager.cpp
 *
 *  Created on: Jan 23, 2018
 *      Author: vrobot
 */

#include <stdint.h>
#include <GL/gl.h>
#include "texture_manager.h"

namespace Fx
{

struct FxTextureId
{
	HTEXTURE			tex;
	int					width;
	int					height;
	CTextureList*		next;
};


/*
HTEXTURE CALL TextureManager::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ = (CRenderTargetList *) target;
	if(target) return (HTEXTURE) targ->pTex;
	else return 0;
}*/

FxTextureId CALL TextureManager::Texture_Create(int width, int height)
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

	return (FxTextureId) pTex;
}

FxTextureId CALL TextureManager::Texture_Load(const char *filename, uint32_t size, bool bMipmap)
{
	int width, height;
	void *data;
	uint32_t _size;
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
	texItem->tex =   (FxTextureId) pTex;
	texItem->width =  width;
	texItem->height = height;
	texItem->next =   textures;
	textures = texItem;

	return (FxTextureId) pTex;
}

void CALL TextureManager::Texture_Free(FxTextureId tex)
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

int CALL TextureManager::Texture_GetWidth(FxTextureId tex, bool bOriginal)
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


int CALL TextureManager::Texture_GetHeight(FxTextureId tex, bool bOriginal)
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


int32_t* CALL TextureManager::Texture_Lock(FxTextureId tex, bool bReadOnly, int left, int top, int width, int height)
{
	// not yet implemented
	return NULL;
}


void CALL TextureManager::Texture_Unlock(FxTextureId tex)
{
	// not yet implemented
}

}
