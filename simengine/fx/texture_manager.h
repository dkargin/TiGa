#pragma once

#include "basetypes.h"

namespace Fx
{

/**
 * TextureManager class
 * Wraps up texture operations
 */
class TextureManager
{
public:
	FxTextureId create(int width, int height);
	FxTextureId loadFile(const char *filename, bool bMipmap=false);
	FxTextureId loadData(const void *data, uint32_t size, bool bMipmap=false);
	void free(FxTextureId tex);
	int width(FxTextureId tex, bool bOriginal=false);
	int height(FxTextureId tex, bool bOriginal=false);
	/*
	uint32_t* Texture_Lock(FxTextureId tex, bool bReadOnly, int left, int top, int width, int height);
	void Texture_Unlock(FxTextureId tex);
	*/
};

}
