/*
 * texture_manager.cpp
 *
 *  Created on: Jan 23, 2018
 *      Author: vrobot
 */

#include <stdint.h>
#include <GL/gl.h>
#include <SDL_image.h>
#include "texture_manager.h"

namespace Fx
{

struct CTextureList
{
	FxTextureId tex;
	int width;
	int height;
	CTextureList* next;
};


/*
HTEXTURE CALL TextureManager::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ = (CRenderTargetList *) target;
	if(target) return (HTEXTURE) targ->pTex;
	else return 0;
}*/

FxTextureId TextureManager::create(int width, int height)
{
	GLuint  pTex;
	int fixed_width = width;//_FixedTextureSize(width);
	int fixed_height = height;//_FixedTextureSize(height);
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

FxTextureId TextureManager::loadFile(const char* filename, bool bMipmap)
{
	int width, height;
	void *data;
	uint32_t _size;
	CTextureList *texItem;

	/*
	// If I want to load from the memory
	SDL_RWops *rw = SDL_RWFromMem(buff,size );
	SDL_Surface *temp = IMG_Load_RW(rw, 1);
	// If I want to convert pixel formats
	optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
	*/

	int uiImageWidth, uiImageHeight, uiImageChannels;
	GLuint pTex;

	SDL_Surface* surf = IMG_Load( filename );

	if (surf == nullptr)
		return 0;

	//unsigned char *PixelData = surf->pixels;//SOIL_load_image_from_memory((unsigned char *) data, _size, &uiImageWidth, &uiImageHeight, &uiImageChannels, SOIL_LOAD_RGBA);

	int colormode = GL_RGB;

	if(surf->format->BytesPerPixel == 4)
		colormode = GL_RGBA;

	GLint prevTex;	//< Need this to restore previous texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);

	glGenTextures(1, &pTex);
	glBindTexture(GL_TEXTURE_2D, pTex);

	glTexImage2D(GL_TEXTURE_2D, 0, colormode, surf->w, surf->h, 0, colormode, GL_UNSIGNED_BYTE, surf->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// TODO: We can need to keep this data in RAM
	SDL_FreeSurface( surf );

	width = uiImageWidth;
	height = uiImageHeight;

	glBindTexture(GL_TEXTURE_2D, prevTex);
	return (FxTextureId) pTex;
}

FxTextureId TextureManager::bind(FxTextureId tex)
{
	GLint prevTex = 0;	//< Need this to restore previous texture
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);
	glBindTexture(GL_TEXTURE_2D, tex);
	return (FxTextureId) prevTex;
}

void TextureManager::free(FxTextureId tex)
{
	GLuint id = tex;
	if (id)
		glDeleteTextures(1, &id);
}

int TextureManager::width(FxTextureId tex, bool bOriginal)
{
	int width = 0;
	GLuint pTex = (GLuint) tex;

	if (bOriginal)
	{
		/*
		CTextureList *texItem = textures;
		while (texItem && width == 0)
		{
			if (texItem->tex == tex)
			{
				width = texItem->width;
			}

			texItem = texItem->next;
		}*/
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

int TextureManager::height(FxTextureId tex, bool bOriginal)
{
	GLuint pTex = (GLuint) tex;
	int height = 0;

	if (bOriginal)
	{
		/*
		CTextureList *texItem = textures;
		while (texItem && height == 0)
		{
			if (texItem->tex == tex)
			{
				height = texItem->height;
			}

			texItem = texItem->next;
		}*/
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

/*
int32_t* TextureManager::Texture_Lock(FxTextureId tex, bool bReadOnly, int left, int top, int width, int height)
{
	// not yet implemented
	return nullptr;
}


void TextureManager::Texture_Unlock(FxTextureId tex)
{
	// not yet implemented
}
*/

}
