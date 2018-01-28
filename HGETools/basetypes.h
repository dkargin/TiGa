#pragma once
#include <stdint.h>
#include <math3/math.h>

#include "rect.h"
#include "color.h"

namespace Fx
{

/*
** HGE Blending constants
*/
#define	BLEND_COLORADD		1
#define	BLEND_COLORMUL		0
#define	BLEND_ALPHABLEND	2
#define	BLEND_ALPHAADD		0
#define	BLEND_ZWRITE		4
#define	BLEND_NOZWRITE		0

#define BLEND_DEFAULT		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE)
#define BLEND_DEFAULT_Z		(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_ZWRITE)


typedef float Time_t;
typedef uint32_t FxRawColor;

inline FxRawColor MakeARGB(int a, int r,int g, int b)
{
	return
		static_cast<FxRawColor>((a&0xff)<<24) |
		static_cast<FxRawColor>((r&0xff)<<16) |
		static_cast<FxRawColor>((g&0xff)<<8) |
		static_cast<FxRawColor>((b&0xff));
}

inline FxRawColor MakeRGB(int r,int g, int b)
{
	return MakeARGB(255, r, g, b);
}


typedef int32_t FxTextureId;

/*
** HGE Vertex structure
*/
struct Vertex
{
	float x, y;			// screen position
	float z;				// Z-buffer depth 0..1
	FxRawColor col;	// color
	float tx, ty;		// texture coordinates
};
/*
** HGE Triple structure
*/
struct Triple
{
	Vertex v[3];
	FxTextureId tex;
	int blend;
};


/*
** HGE Quad structure
*/
struct Quad
{
	Vertex v[4];
	FxTextureId tex;
	int blend;
};

typedef math3::Pose2z Pose;
}
