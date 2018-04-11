#pragma once

#include <stdint.h>
#include <chrono>
#include <math3/math.h>
#include <stdint.h>
#include <vector>

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

	static Vertex make2c(float x, float y, FxRawColor color, float tx = 0, float ty = 1)
	{
		return Vertex{x, y, 0.f, color, tx, ty};
	}

	static Vertex make3c(float x, float y, float z, FxRawColor color, float tx = 0, float ty = 1)
	{
		return Vertex{x, y, z, color, tx, ty};
	}
};

// Batch with a vertices
struct VertexBatch
{
	// Primitive type constants
	enum PrimType
	{
		PRIM_INVALID = 0,
		PRIM_POINTS = 1,
		PRIM_LINES = 2,
		PRIM_TRIPLES = 3,
		PRIM_QUADS = 4,
	};

	VertexBatch(PrimType prim)
	:primType(prim)
	{
	}

	int primType = PRIM_INVALID;
	int blend = 0;
	FxTextureId texture = -1;
	// Number of generated primitives
	int prims = 0;
	std::vector<Vertex> buffer;

	VertexBatch& operator += (std::initializer_list<Vertex>&& data)
	{
		append(std::move(data));
		return *this;
	}

	VertexBatch& operator += (std::vector<Vertex>&& data)
	{
		append(std::move(data));
		return *this;
	}

	// Append vertex data to a batch
	int append(std::initializer_list<Vertex>&& data)
	{
		if (primType == PRIM_INVALID)
			return 0;

		// Data length should correspond to current primitive type
		int size = data.size();
		if (size % primType)
			return 0;

		buffer.insert(buffer.end(), std::move(data));
		prims += size / primType;
		return buffer.size();
	}

	// Append vertex data to a batch
	int append(std::vector<Vertex>&& data)
	{
		// Data length should correspond to current primitive type
		int size = data.size();
		if (size % primType)
			return 0;

		buffer.insert(buffer.end(), data.begin(), data.end());
		prims += size / primType;
		return buffer.size();
	}
};

typedef math3::Pose2z Pose;
typedef math3::vec2f vec2f;
typedef math3::vec3 vec3;

// Context for game update
struct UpdateContext
{
	typedef std::chrono::time_point<std::chrono::system_clock> time_point;
	// System time point
	time_point system_time;
	// Local time point, calculated from the start of application or game logic
	time_point local_time;
	// Delta time from last update, in seconds
	std::chrono::duration<float> delta;	// delta time
	// Index of current frame
	int64_t frame;

	float getDelta() const;
	int32_t getDeltaMs() const;
};

}
