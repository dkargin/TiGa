/*
 * texture_manager.cpp
 *
 *  Created on: Jan 23, 2018
 *      Author: vrobot
 */

#include <stdint.h>
#include <GL/gl.h>
#include <SDL_image.h>
#include "texturemanager.h"

namespace Fx
{

TextureManager::TextureData* TextureManager::createTextureData()
{
	GLuint tex = 0;
	glGenTextures(1, &tex);
	auto it = textures.find(tex);
	if (it != textures.end())
	{
		// This is strange.
		it->second.refs++;
		return &it->second;
	}
	TextureData& data = textures[tex];
	data.texture = tex;
	data.refs = 1;
	return &data;
}

FxTextureId TextureManager::create(int width, int height, int format, int channels)
{
	int fixedWidth = width;
	int fixedHeight = height;
	int pixelSize = sizeof(unsigned int) * channels;

	// Total size in bytes
	int totalSize = fixedWidth * fixedHeight * pixelSize;

	std::unique_ptr<unsigned int> data(new GLuint[totalSize]);
	memset(data.get(), 0, totalSize * sizeof(unsigned int));

	TextureData* td = createTextureData();
	if (!td)
		return 0;

	glBindTexture(GL_TEXTURE_2D, td->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, fixedWidth, fixedHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.get());
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	td->width = fixedWidth;
	td->height = fixedHeight;

	return (FxTextureId) td->texture;
}

FxTextureId TextureManager::loadFile(const char* filename, bool bMipmap)
{
	/*
	// If I want to load from the memory
	SDL_RWops *rw = SDL_RWFromMem(buff,size );
	SDL_Surface *temp = IMG_Load_RW(rw, 1);
	// If I want to convert pixel formats
	optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL );
	*/

	int uiImageWidth, uiImageHeight, uiImageChannels;

	std::unique_ptr<SDL_Surface, void (*)(SDL_Surface*)> surf(IMG_Load(filename), SDL_FreeSurface);

	if (surf == nullptr)
		return 0;

	TextureData* td = createTextureData();
	if (!td)
		return 0;

	glBindTexture(GL_TEXTURE_2D, td->texture);

	//unsigned char *PixelData = surf->pixels;//SOIL_load_image_from_memory((unsigned char *) data, _size, &uiImageWidth, &uiImageHeight, &uiImageChannels, SOIL_LOAD_RGBA);

	int colormode = GL_RGB;

	if(surf->format->BytesPerPixel == 4)
		colormode = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, colormode, surf->w, surf->h, 0, colormode, GL_UNSIGNED_BYTE, surf->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// TODO: We can need to keep this data in RAM
	//SDL_FreeSurface( surf );

	td->width = uiImageWidth;
	td->height = uiImageHeight;

	return (FxTextureId) td->texture;
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

int TextureManager::width(FxTextureId tex) const
{
	int width = 0;
	GLuint pTex = (GLuint) tex;

	GLfloat fWidth;
	GLint curTex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);

	glBindTexture(GL_TEXTURE_2D, pTex);
	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &fWidth);

	glBindTexture(GL_TEXTURE_2D, curTex);

	width = (int) fWidth;

	return width;
}

int TextureManager::height(FxTextureId tex) const
{
	GLuint pTex = (GLuint) tex;
	int height = 0;

	GLfloat fHeight;
	GLint curTex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);

	glBindTexture(GL_TEXTURE_2D, pTex);
	glGetTexLevelParameterfv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &fHeight);

	glBindTexture(GL_TEXTURE_2D, curTex);

	height = (int) fHeight;

	return height;
}

}
