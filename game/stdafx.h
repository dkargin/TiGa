// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#error "Game does not use stdafx.h"

#ifdef _WINDOWS
	#define _CRTDBG_MAP_ALLOC
	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	// Windows Header Files:
	#include <windows.h>

	#ifdef _DEBUG
		#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
		#endif
	#endif  // _DEBUG
#endif //_WINDOWS

#include <list>
#include <vector>
#include <string>
#include <map>
#include <memory>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory>


#define LUABOX_BINDER

#include <Box2D/Box2D.h>
#include <math3/3dmath.h>
#include <math3/ioTools.h>
#include <math3/iobuffer.h>

#define FrostTools_NoWarnings
#define FrostTools_Use_Asserts
#define FrostTools_Use_System
#define FrostTools_Use_Threads

/*
#include <../frosttools/logger.hpp>
#include <../frostTools/frostTools.h>
#include <../frosttools/autoptr.hpp>
#include <../frosttools/raster.hpp>
#include <../frosttools/sysFiles.hpp>
 
#include <pathProject.h>
#include <pathDrawer.h>
#include <hge.h>
#include <hgegui.h>
#include <hgefont.h>
#include <hgecolor.h>
#include <hgesprite.h>
#include <hgeparticle.h>
#include <hgeresource.h>
#include <hgeguictrls.h>
*/

#define LUABOX_BINDER_SWIG
#include "../sim/luabox/scripter.hpp"

bool pushObjectPtr(lua_State *l,void *object,const char *name);

template<class Type> bool pushObject(lua_State *l,Type *object)
{
	return pushObjectPtr(l,object,typeName(object));
}

Pose2 GetPose(const b2Body * body);

#define DEVICE_RENDER
