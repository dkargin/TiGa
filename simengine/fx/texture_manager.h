#pragma once

#include "basetypes.h"

#define CALL

namespace Fx
{

/**
 * TextureManager class
 * Wraps up texture operations
 */
class TextureManager
{
public:
	FxTextureId Texture_Create(int width, int height);
	FxTextureId Texture_Load(const char *filename, uint32_t size=0, bool bMipmap=false);
	void CALL Texture_Free(FxTextureId tex);
	int CALL Texture_GetWidth(FxTextureId tex, bool bOriginal=false);
	int CALL Texture_GetHeight(FxTextureId tex, bool bOriginal=false);
	uint32_t* CALL Texture_Lock(FxTextureId tex, bool bReadOnly, int left, int top, int width, int height);
	void CALL Texture_Unlock(FxTextureId tex);
};

}
