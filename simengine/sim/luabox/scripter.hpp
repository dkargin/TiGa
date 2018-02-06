#pragma once

#include <sstream>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "bindTypes.h"
#include <typeinfo>
#include <string.h>

namespace LuaBox
{
	class Scripter;
	
	inline void stackDump (lua_State *L) 
	{
		int i;
		int top = lua_gettop(L);
		for (i = 1; i <= top; i++) 
		{  /* repeat for each level */
			int t = lua_type(L, i);
			switch (t) 
			{
			case LUA_TSTRING:  /* strings */
				printf("`%s'", lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:  /* booleans */
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:  /* numbers */
				printf("%g", lua_tonumber(L, i));
				break;
			default:  /* other values */
				printf("%s", lua_typename(L, t));
				break;
			}
			printf("  ");  /* put a separator */
		}
		printf("\n");  /* end the listing */
	}

	/////////////////////////////////////////////////////////////
	/// Helper object, maintains bidirectional link with C++ object 
	/// and Lua table, stored in lua registry. Peforms binding and access
	/////////////////////////////////////////////////////////////
	class LuaObject
	{
	protected:	
		// long way to get lua state
		//inline lua_State * getLua();
	public:
		int lua;	/// registry reference
		LuaObject();
		virtual ~LuaObject() {}
		/// stores pointer to C++ object in lua table
		//void attachData();
		/// get object from registry
		void getObject(lua_State *state);
		/// remove object from registry
		void unbind(lua_State *state);
		/// attach to existing registry object
		void bind(lua_State *state, int ref);
		/// get type name
		virtual const char *luaName()const;
		//virtual _Scripter *getScripter()=0;
		//void callOnInit();
	};
	//template<> inline const char * typeName<LuaObject>(LuaObject *ptr)
	//{
	//	return ptr->luaName();
	//}
	////////////////////////////////////////////////////////////
	// LuaObject proxy, uses RTTI to determine type name
	////////////////////////////////////////////////////////////
	//template<typename Target> class LuaHelper: virtual public LuaObject
	//{
	//public:
	//	virtual const char *luaName()const
	//	{
	//		return typeName<Target>();
	//	}
	//};

	// Scoped object to keep stack safe. Not so safe
	struct StackSentry
	{
		int top;
		lua_State *l;
		StackSentry(lua_State *l)
			:l(l)
		{
			top=lua_gettop(l);
		}
		~StackSentry()
		{
			lua_settop(l,top);
		}
	};
	////////////////////////////////////////////////////////////////////////
	// lua_State wrapper. Makes interaction with lua a bit simplier and a byte harder
	////////////////////////////////////////////////////////////////////////
	class Scripter
	{		
	public:	
		typedef int luaRef;
		class ErrorHandler
		{
			friend class Scripter;
		protected:
			virtual bool onScripterError(Scripter &scripter,const char *error)=0;
			//virtual bool lineHook(_Scripter &scripter)=0;
		}*errorHandler;

		class Object;

		Scripter(lua_State *state = NULL);

		virtual ~Scripter();

		// Attach to lua state
		bool attach(lua_State * state = NULL);

		void prepareWrapper();

		bool runScript(const char * file, bool debug = false);
		/// get object from registry
		Scripter::Object getRegistry(luaRef ref);
		/// get object from global namespace
		Scripter::Object getGlobals(const char *object);
		// get table field with string key <fieldName>
		Scripter::Object getTable(const char *fieldName,int tableIndex=-2);

		template<class Type> void addGlobal(const Type &type,const char *name);

		const char * getError() const;

		lua_State* getVM(){return L;}
		/// execute the call, assuming the function and its arguments are already in lua stack
		/// Own error handler is inserted before actual call
		static bool doCall(lua_State * L, int arg, bool debug = false);

		operator lua_State*()
		{
			return L;
		}		

		bool call(const char * fn)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,NullType(),NullType(),NullType(),NullType(),NullType(),NullType(),NullType(),NullType());
		}	
		template<typename T1>
		bool call(const char * fn,T1 arg1)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,NullType(),NullType(),NullType(),NullType(),NullType(),NullType(),NullType());
		}
		template<typename T1,typename T2>
		bool call(const char * fn,T1 arg1,T2 arg2)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,arg2,NullType(),NullType(),NullType(),NullType(),NullType(),NullType());
		}
		template<typename T1,typename T2,typename T3>
		bool call(const char * fn,T1 arg1,T2 arg2,T3 arg3)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,arg2,arg3,NullType(),NullType(),NullType(),NullType(),NullType());
		}
		template<typename T1,typename T2,typename T3,typename T4>
		bool call(const char * fn,T1 arg1,T2 arg2,T3 arg3,T4 arg4)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,arg2,arg3,arg4,NullType(),NullType(),NullType(),NullType());
		}
		template<typename T1,typename T2,typename T3,typename T4,typename T5>
		bool call(const char * fn,T1 arg1,T2 arg2,T3 arg3,T4 arg4,T5 arg5)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,arg2,arg3,arg4,arg5,NullType(),NullType(),NullType());
		}
		template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
        bool call(const char * fn,T1 arg1,T2 arg2,T3 arg3,T4 arg4,T5 arg5,T6 arg6)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,arg2,arg3,arg4,arg5,arg6,NullType(),NullType());
		}
		template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
		bool call(const char * fn,T1 arg1,T2 arg2,T3 arg3,T4 arg4,T5 arg5,T6 arg6,T7 arg7)
		{
			StackSentry sentry(getVM());
			return getFunction(fn) && pcall(L,arg1,arg2,arg3,arg4,arg5,arg6,arg7,NullType());
		}

		// Get scripter pointer from lua vm
		static Scripter * getScripter(lua_State *l);
	protected:
		static int _s_errorHook(lua_State *l);
		static void _s_lineHook(lua_State *l, lua_Debug *ar);
		
		// Tries to get function from globals
		bool getFunction(const char *fn);
		
		virtual void onError();

		// line hook handler
		virtual void onHook(int type,lua_Debug *ar);
		// reads error from lua
		void _process_error();

		template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
		static bool pcall(lua_State * L, const T1 &arg1,const T2 &arg2,const T3 &arg3,const T4 &arg4,const T5 &arg5,const T6 &arg6,const T7 &arg7,const T8 &arg8)
		{
			int res=0;
			res+=LuaBox::TypeIO<T1>::write(L,arg1);
			res+=LuaBox::TypeIO<T2>::write(L,arg2);
			res+=LuaBox::TypeIO<T3>::write(L,arg3);
			res+=LuaBox::TypeIO<T4>::write(L,arg4);
			res+=LuaBox::TypeIO<T5>::write(L,arg5);
			res+=LuaBox::TypeIO<T6>::write(L,arg6);
			res+=LuaBox::TypeIO<T7>::write(L,arg7);
			res+=LuaBox::TypeIO<T8>::write(L,arg8);
			return doCall(L, res, false);
		}
		
		std::string error,errorCallStack,errorMessage;
		lua_State* L;
		int errFunc;
		bool externVM;	// was attached to external VM?

		enum ScriptState
		{
			stateOffline,
			stateReady,
			stateErrorLocal,	
		}state;
	};

	////////////////////////////////////////////////////////////////////////////////////
	typedef Scripter Scripter;

	
	// Stores all data in LUA vm
	class LuaTable
	{
		std::string name;
		lua_State *l;
	public:	
		LuaTable(lua_State *state,const char *name)
			:name(name),l(state)
		{
			lua_newtable(l);
			lua_setglobal(l,name);
		}
		virtual ~LuaTable()
		{
			lua_pushnil(l);
			lua_setglobal(l,name.c_str());
		}

		std::string get(const char *key)
		{
			lua_getglobal(l,name.c_str());
			lua_pushstring(l,key);
			lua_gettable(l,-2);
			std::string result=lua_tostring(l,-1);
			lua_pop(l,2);
			return result;
		}

		void set(const char *key,const char *value)
		{
			lua_getglobal(l,name.c_str());
			lua_pushstring(l,key);
			lua_pushstring(l,value);
			lua_settable(l,-3);
			lua_pop(l,1);
		}
	};
}	// namespace LuaBox

//#include "../luabox/scripter.inl"

