#pragma once

#include <sstream>

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "bind_base.h"
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

/*
 * Helper object, maintains bidirectional link with C++ object
 * and Lua table, stored in lua registry. Peforms binding and access
 *
 * Used as base class for the types, intended to be used in lua
 */
class LuaObject
{
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
	virtual const char *luaName() const;
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

	template<typename ... Ts>
	bool call(const char* name, Ts... args)
	{
		StackSentry sentry(getVM());
		if (!getFunction(name))
			return false;
		int argnum = pushStack(L, args...);
		return doCall(L, argnum, false);
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


/*
 * Scripter::Object is proxy for lua stack object.
 * It is used to interact with objects from LUA VM.
 */

class Scripter::Object
{
	friend class Scripter;
	lua_State * l;
	Object * parent;
	int registry;
	size_t * refCounter;

	// Can only be constructed by the scripter
	Object(lua_State * l);
	Object(const Object &obj);
public:
	~Object();

	bool call(const char* name)
	{
		//StackSentry sentry(getVM());
		if (!getFunction(name))
			return false;
		int argnum = pushStack(l, this);
		return Scripter::doCall(l, argnum, false);
	}

	template<typename ... Ts>
	bool call(const char* name, Ts... args)
	{
		//StackSentry sentry(getVM());
		if (!getFunction(name))
			return false;
		int argnum = pushStack(l, this, args...);
		return Scripter::doCall(l, argnum, false);
	}

	// Check if object is valid
	bool valid() const;
	// Places object from registry to stack
	void toStack();
	// Construct from stack value
	static Object fromStack(lua_State * l, int stack = -1);
	// Construct from registry
	static Object fromRegistry(lua_State * l, int registry);
private:
	// Get method and place it to stack
	bool getFunction(const char * fn);
};

////////////////////////////////////////////////////////////////////////////////////
// _LuaObject implementation
////////////////////////////////////////////////////////////////////////////////////

//inline void LuaObject::callOnInit()
//{
////	LogFunction(*g_logger);
//	if(!lua)return;
//	_Scripter *scripter=getScripter();
//	lua_State *l=scripter->getVM();
//	// get lua object from stack
//	_Scripter::Object object=scripter->getRegistry(lua);
//	// call lua:onInit()
//	lua_pushstring(l,"onInit");
//	lua_gettable(l,-2);				// get onCreate method
//	lua_pushvalue(l,-2);			// arg0
//	scripter->doCall(l,1);
//}
////////////////////////////////////////////////////////////////////////////////////
// _Scripter implementation
////////////////////////////////////////////////////////////////////////////////////

template<class Type> inline void Scripter::addGlobal(const Type &value,const char *name)
{
	LuaBox::TypeBindings<Type>::write(L, value);
	lua_setglobal(L, name);
}

template<> struct TypeBindings<Scripter::Object*>
{
	static inline bool write(Stream *L,_Scripter::Object * value)
	{
		value->toStack();
		return 1;
	}
	static inline Scripter::Object* get(Stream *l,int i)
	{
		return NULL;
	}
};

template<> struct TypeBindings<LuaObject*>
{
	typedef LuaObject* value_type;
	static bool valid(lua_State *l,int i)
	{
		int type=lua_type(l,i);
		return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
	}

	static int write(Stream *luaVM, value_type value)
	{
		return pushObjectPtr(luaVM,value, value->luaName())?1:0;
	}
private:
	virtual value_type get(Stream *l,int i)=0;
	virtual ~TypeBindings(){}
};

}	// namespace LuaBox
