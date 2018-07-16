#pragma once

#include <ioTools.h>			//< from frosttools
#include <iobuffer.h>			//< from frosttools
#include <math3/math.h>
#include <math3/mathExtensions.hpp>
#include "luabox/scripter.h"


/*
 * Header contains primitive type definitions
 */



namespace sim
{

// Wrapping types from math3
typedef math3::Pose2z Pose;
using vec2f = math3::vec2f;
using vec3f = math3::vec3f;
using Trajectory2 = math3::geometry::Trajectory2;

typedef math3::geometry::_Sphere<vec2f> Sphere2;
typedef math3::geometry::_Edge<vec2f> Edge2;
typedef math3::geometry::_AABB<vec2f> AABB2;

// Game-specific stuff
typedef float Damage;
typedef float Health;

typedef unsigned int ID;
const ID invalidID = -1;
typedef ID DefID;	// object definition identifier
typedef ID ObjID;	// object identifier

// Let's summon some byte streams
using StreamIn = frosttools::IO::StreamIn;
using StreamOut = frosttools::IO::StreamOut;
using IOBuffer = frosttools::IOBuffer;
using Scripter = LuaBox::Scripter;
using LuaObject = LuaBox::LuaObject;

}
