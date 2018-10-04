/*
** Haaf's Game Engine 1.7
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Font helper class header
*/
#pragma once

#include "basetypes.h"
#include "spritedata.h"

namespace Fx
{

enum Alignment
{
	AlignLeft = 0,
	AlignRight = 1,
	AlignCenter = 2,
	AlignTop = 4,
	AlignBottom = 5,
};

#define HGETEXT_LEFT		0
#define HGETEXT_RIGHT		1
#define HGETEXT_CENTER		2
#define HGETEXT_HORZMASK	0x03

#define HGETEXT_TOP			0
#define HGETEXT_BOTTOM		4
#define HGETEXT_MIDDLE		8
#define HGETEXT_VERTMASK	0x0C


/*
** Font class
** Wraps up TTF font
*/
class Font
{
public:
	/**
	 * filename - path to TTF font
	 */
	Font(const char *filename, bool bMipmap=false);
	~Font();

	// Render font to a vertex batch. This batch should be rendered separatly.
	VertexBatch Render(float x, float y, int align, const char *string);
	VertexBatch printf(float x, float y, int align, const char *format, ...);
	VertexBatch printfb(float x, float y, float w, float h, int align, const char *format, ...);

	void SetColor(FxRawColor col);
	void SetZ(float z);
	void SetBlendMode(int blend);
	void SetScale(float scale) {fScale=scale;}
	void SetProportion(float prop) { fProportion=prop; }
	void SetRotation(float rot) {fRot=rot;}
	void SetTracking(float tracking) {fTracking=tracking;}
	void SetSpacing(float spacing) {fSpacing=spacing;}

	FxRawColor GetColor() const {return dwCol;}
	float GetZ() const {return fZ;}
	int GetBlendMode() const {return nBlend;}
	float GetScale() const {return fScale;}
	float GetProportion() const { return fProportion; }
	float GetRotation() const {return fRot;}
	float GetTracking() const {return fTracking;}
	float GetSpacing() const {return fSpacing;}

	SpriteData* GetSprite(char chr) const { return letters[(unsigned char)chr]; }
	float GetPreWidth(char chr) const { return pre[(unsigned char)chr]; }
	float GetPostWidth(char chr) const { return post[(unsigned char)chr]; }
	float GetHeight() const { return fHeight; }
	float GetStringWidth(const char *string, bool bMultiline=true) const;

private:
	Font();
	Font(const Font &fnt);
	Font& operator= (const Font &fnt);

	char* _get_line(char *file, char *line);

	FxTextureId hTexture;
	//std::map<>
	SpriteData* letters[256];
	float pre[256];
	float post[256];
	float fHeight;
	float fScale;
	float fProportion;
	float fRot;
	float fTracking;
	float fSpacing;

	FxRawColor dwCol;
	float fZ;
	int nBlend;

	struct Letter
	{
		FxTextureId texture;
	};

	// Buffer for string operations. Will we overflow it?
	char buffer[1024];
};

} // namespace Fx
