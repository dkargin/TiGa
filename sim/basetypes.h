#pragma once

#include <ioTools.h>
#include <math3/math.h>
#include <math3/mathExtensions.hpp>
#include "luabox/scripter.hpp"

/*
 * Header contains primitive type definitions
 */



namespace sim
{

// Wrapping types from math3
typedef math3::Pose2z Pose;
using vec2f = math3::vec2f;
using vec3f = math3::vec3f;
typedef float Damage;
typedef math3::geometry::_Sphere<vec2f> Sphere2;
typedef math3::geometry::_AABB<vec2f> AABB2;

typedef unsigned int ID;
const ID invalidID = -1;
typedef ID DefID;	// object definition identifier
typedef ID ObjID;	// object identifier

// Let's summon some byte streams
using StreamIn = frosttools::IO::StreamIn;
using StreamOut = frosttools::IO::StreamOut;

using Scripter = LuaBox::Scripter;
}
