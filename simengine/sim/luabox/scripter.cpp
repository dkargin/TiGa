#include "scripter.hpp"
#include "lua.h"
#include "lauxlib.h"

#include <cassert>

namespace LuaBox
{

Scripter::Scripter(lua_State *state)
	:L(0),state(stateOffline),errFunc(0),externVM(false),errorHandler(NULL)
{
	attach(state);
}

Scripter::~Scripter()
{
	if(!externVM && L)
		lua_close( L );
}

bool Scripter::attach(lua_State * state)
{
	if(state == NULL)
	{
		L = luaL_newstate();
		externVM = false;
	}
	else
	{
		assert(L != state);
		assert(getScripter(state) == NULL);
		L = state;
		externVM = true;
	}
	prepareWrapper();
	return true;
}

void Scripter::prepareWrapper()
{
	lua_pushlightuserdata(L,this);
	lua_setglobal(L,"Scripter");
	state = stateReady;
	lua_sethook(L,_s_lineHook, LUA_MASKCALL|LUA_MASKRET,0);
	//errFunc = lua_gethook(L);
	luaopen_debug(L);
	luaL_openlibs(L);
}

bool Scripter::runScript(const char * file, bool debug)
{
	if(!L)
		return false;
	if(luaL_loadfile(L,file))
	{
		errorMessage = std::string("error loading file <")+file+
			std::string(">:").append(lua_tostring(L,-1));
		lua_pop(L,1);
		_process_error();
		return false;
	}
	error="";
	return doCall(L,0,debug);
}
const char * Scripter::getError() const
{
	return errorMessage.c_str();
}

/// execute the call, assuming the function and its arguments are already in lua stack
bool Scripter::doCall(lua_State * L, int arg, bool debug)
{
	int error_index = 0;
	if(debug)
	{
		lua_pushcfunction(L, &_s_errorHook);
		lua_gettable(L,LUA_REGISTRYINDEX);
		lua_insert(L, -arg - 2);
		error_index = -arg - 2;
	}
	/* call func with error handler */
	if (lua_pcall(L, arg, 0, error_index)!= 0)
	{
		if( !debug )
		{
			Scripter * scripter = getScripter(L);
			scripter->_process_error();
		}
		error_index++;
		return false;
	}
	if(debug)
		lua_pop(L,1);
	return true;
}

bool Scripter::getFunction(const char* name)
{
	int index = lua_getglobal(L, name);
	lua_pushvalue(L, index);
	return lua_type(L,-1) == LUA_TFUNCTION;
}

Scripter * Scripter::getScripter(lua_State *l)
{
	lua_getglobal(l,"Scripter");
	Scripter *scripter=NULL;
	if(lua_type(l,-1)==LUA_TLIGHTUSERDATA)
		scripter=(Scripter*)lua_touserdata(l,-1);
	lua_pop(l,1);
	return scripter;
}

int Scripter::_s_errorHook(lua_State *l)
{
	Scripter *scripter=getScripter(l);
	if(!scripter)
		printf("errorHook: bad Scripter entry\n");
	else
		scripter->onError();
	return 0;
}

void Scripter::_s_lineHook(lua_State *l, lua_Debug *ar)
{
	Scripter *scripter=getScripter(l);
	if(!scripter)
		printf("lineHook: bad Scripter entry\n");
	else
		scripter->onHook(0,ar);
}

void Scripter::onError()
{
	const int maxStack=16;
	lua_Debug d;
	errorCallStack.clear();
	size_t len=0;
	const char *es=lua_tolstring(L,-1,&len);
	if(es)error=es;
	std::ostringstream stream;
	for (int last=1;last<maxStack;last++)
	{
		if (lua_getstack(L,last,&d))
		{
			if (!lua_getinfo(L, "Sln",&d))
				break;
			if (*d.what=='C')
				d.name="<C code>";
			if (d.name==0)
				d.name="?";
			stream<<" "<<d.name<<"("<<d.currentline<<"):"<<d.short_src<<std::endl;
		}
	}
	errorCallStack=stream.str();
	errorMessage=error;
	if(errorHandler)
		errorHandler->onScripterError(*this,getError());
}

void Scripter::onHook(int type,lua_Debug *ar)
{

}

void Scripter::_process_error()
{
	size_t len = 0;
	const char *es=lua_tolstring(L,-1,&len);
	if(es)
		error=es;
	errorMessage = error;
	if(errorHandler)
		errorHandler->onScripterError(*this,getError());
	return;
}

inline Scripter::Object Scripter::getRegistry(Scripter::luaRef ref)
{
	return Object::fromRegistry(*this, ref);
}

inline Scripter::Object Scripter::getGlobals(const char *name)
{
	StackSentry sentry(getVM());
	int index = lua_getglobal(L, name);
	lua_pushvalue(L, index);
	return Object::fromStack(*this);
}

// get table field with string key <fieldName>
Scripter::Object Scripter::getTable(const char *fieldName, int tableIndex)
{
	StackSentry sentry(getVM());
	lua_pushstring(L,fieldName);
	lua_gettable(L,tableIndex);
	return Object::fromStack(*this);
}

//////////////////////////////////////////////////////////////////////////////////
// LuaObject implementation
//////////////////////////////////////////////////////////////////////////////////
LuaObject::LuaObject():lua(0) {}
/*
lua_State * LuaObject::getLua()
{
	return getScripter()->getVM();
}
*/
void LuaObject::getObject(lua_State *state)
{
	lua_rawgeti(state,LUA_REGISTRYINDEX,lua);
}

void LuaObject::unbind(lua_State *state)
{
	if(lua)luaL_unref(state, LUA_REGISTRYINDEX,lua);
	lua=0;
}

void LuaObject::bind(lua_State *state, int ref)
{
	assert(lua!=0 && ref!=lua);
	if(ref!=lua)
	{
		unbind(state);
		lua=ref;
	}
}

const char *LuaObject::luaName()const
{
	return typeName(this);
}

//////////////////////////////////////////////////////////////////////////////////
// Object
//////////////////////////////////////////////////////////////////////////////////


Scripter::Object::Object(lua_State * l)
:l(l),parent(NULL),registry(0),refCounter(NULL)
{}

Scripter::Object::Object(const Object &obj)
	:l(obj.l), refCounter(obj.refCounter), registry(obj.registry), parent(obj.parent)
{
	if(refCounter)
		(*refCounter)++;
}

Scripter::Object::~Object()
{
	if( refCounter && --(*refCounter) )
	{
		delete refCounter;
		refCounter = NULL;
		if( registry )
		{
			luaL_unref(l, LUA_REGISTRYINDEX, registry);
			registry = 0;
		}
	}
}

bool Scripter::Object::valid() const
{
	return l != NULL && registry != 0 && refCounter != 0;
}
// places object from registry to stack
void Scripter::Object::toStack()
{
	lua_rawgeti(l, LUA_REGISTRYINDEX, registry);
}

//  construct from stack value
Scripter::Object Scripter::Object::fromStack(lua_State* l, int stack)
{
	Object result(l);
	if( lua_type(l, stack) != LUA_TNIL )
	{
		result.refCounter = new size_t(1);
		lua_pushvalue(l, stack);
		result.registry = luaL_ref(l, LUA_REGISTRYINDEX);
	}
	return result;
}

//  construct from registry
Scripter::Object Scripter::Object::fromRegistry(lua_State * l, int registry)
{
	Object result(l);
	result.registry = registry;
	// do not create reference counter
	return result;
}

// get method and place it to stack
bool Scripter::Object::getFunction(const char * fn)
{
	toStack();
	lua_pushstring(l, fn);
	lua_rawget(l, -2);
	lua_remove(l, -2);
	return lua_type(l,-1) == LUA_TFUNCTION;
}

}
