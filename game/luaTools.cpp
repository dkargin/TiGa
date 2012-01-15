#include "stdafx.h"

/// SWIG bind
extern "C" int luaopen_TiGa(lua_State* L);
Log * g_logger = NULL;

/*
void lua_push(lua_State *l,lua_CFunction fn,const char *name)
{
	lua_pushcfunction(l, fn);
    lua_setglobal(l, name);
}
*/
void initLuaWrap( lua_State * l )
{
	luaopen_TiGa(l);	
	/*
	lua_push(l,regStore,"regStore");
	lua_push(l,regGet,"regGet");
	lua_push(l,regFree,"regFree");
	lua_push(l,debugTrace,"debugTrace");
	lua_push(l,binOR,"binOR");
	lua_push(l,binAND,"binAND");
	lua_push(l,binNOT,"binNOT");
	lua_push(l,SWIG_base_name,"SWIG_base_name");
	lua_push(l,convertHandle,"convertHandle");	
	lua_push(l,lua_toAppRelative,"toAppRelative");*/
}