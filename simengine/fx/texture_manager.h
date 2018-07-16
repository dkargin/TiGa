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

	// Binds specified texture and returns the previous one
	FxTextureId bind(FxTextureId tex);
};

}
