#pragma once
#include <stdint.h>
#include <math3/math.h>

#include "rect.h"
#include "color.h"

namespace Fx
{
typedef float Time_t;
typedef int32_t FxRawColor;
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
