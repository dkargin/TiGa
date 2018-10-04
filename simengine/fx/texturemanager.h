#pragma once

#include <map>
#include <memory>
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
	// Creates an empty texture.
	// TODO: Why no format is here?
	FxTextureId create(int width, int height, int format, int channels);
	// Load texture from the file
	FxTextureId loadFile(const char *filename, bool bMipmap=false);
	// Load a texture using raw data
	FxTextureId loadData(const void *data, uint32_t size, bool bMipmap=false);
	void free(FxTextureId tex);

	void addRef(FxTextureId tex);
	void release(FxTextureId tex);

	// Read texture info
	int width(FxTextureId tex) const;
	int height(FxTextureId tex) const;

	// Binds specified texture and returns the previous one
	// TODO: Multitexturing will break it
	FxTextureId bind(FxTextureId tex);

protected:
	// Wraps up texture data.
	struct TextureData
	{
		FxTextureId texture = 0;
		int width = 0, height = 0;
		int refs = 0;
		std::string file;
	};

	TextureData* createTextureData();

	std::map<FxTextureId, TextureData> textures;
};

}
