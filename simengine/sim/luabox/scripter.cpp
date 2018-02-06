#include "../luabox/scripter.hpp"

#include <assert.h>

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
			L = lua_open();
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
		errFunc=lua_sethook(L, _s_lineHook,LUA_MASKCALL|LUA_MASKRET,0);
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
		
	bool Scripter::getFunction(const char *fn)
	{
		lua_getfield(L,LUA_GLOBALSINDEX,fn);
		return lua_type(L,-1)==LUA_TFUNCTION;
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
		lua_getfield(L,LUA_GLOBALSINDEX,name);
		return Object::fromStack(*this);
	}

	// get table field with string key <fieldName>
	inline Scripter::Object Scripter::getTable(const char *fieldName,int tableIndex)
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

}
