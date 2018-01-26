#ifndef FROSTTOOLS_SCRIPTER_INL
#define FROSTTOOLS_SCRIPTER_INL

namespace LuaBox
{
//////////////////////////////////////////////////////////////////////////////////////////
// _Scripter::Object is proxy for lua stack object
//////////////////////////////////////////////////////////////////////////////////////////
class _Scripter::Object
{
	friend class _Scripter;
	lua_State * l;	
	Object * parent;
	int registry;
	size_t * refCounter;
	
	Object(lua_State * l):l(l),parent(NULL),registry(0),refCounter(NULL){}
public:	
	Object(const Object &obj)
		:l(obj.l), refCounter(obj.refCounter), registry(obj.registry), parent(obj.parent)
	{
		if(refCounter)
			(*refCounter)++;
	}
	~Object()
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
	bool call(const char * fn)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,NullType(),NullType(),NullType(),NullType(),NullType(),NullType(),NullType());
	}
	template<typename T1>
	bool call(const char * fn, T1 arg1)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,NullType(),NullType(),NullType(),NullType(),NullType(),NullType());
	}
	template<typename T1,typename T2>
	bool call(const char * fn, T1 arg1, T2 arg2)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,arg2,NullType(),NullType(),NullType(),NullType(),NullType());
	}
	template<typename T1,typename T2,typename T3>
    bool call(const char * fn, T1 arg1, T2 arg2, T3 arg3)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,arg2,arg3,NullType(),NullType(),NullType(),NullType());
	}
	template<typename T1,typename T2,typename T3,typename T4>
	bool call(const char * fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,arg2,arg3,arg4,NullType(),NullType(),NullType());
	}
	template<typename T1,typename T2,typename T3,typename T4,typename T5>
	bool call(const char * fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,arg2,arg3,arg4,arg5,NullType(),NullType());
	}
	template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	bool call(const char * fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,arg2,arg3,arg4,arg5,arg6,NullType());
	}
	template<typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	bool call(const char * fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
	{
		return getFunction(fn) && _Scripter::pcall(l,this,arg1,arg2,arg3,arg4,arg5,arg6,arg7);
	}
	inline bool valid() const
	{
		return l != NULL && registry != 0 && refCounter != 0;
	}
	// places object from registry to stack
	void toStack()
	{
		lua_rawgeti(l, LUA_REGISTRYINDEX, registry);
	}
	//  construct from stack value
	static Object fromStack(lua_State * l, int stack = -1)
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
	static Object fromRegistry(lua_State * l, int registry)
	{
		Object result(l);
		result.registry = registry;
		// do not create reference counter
		return result;
	}
private:	
	// get method and place it to stack
	bool getFunction(const char * fn)
	{
		toStack();		
		lua_pushstring(l, fn);
		lua_rawget(l, -2);		
		lua_remove(l, -2);
		return lua_type(l,-1) == LUA_TFUNCTION;
	}
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

template<class Type> inline void _Scripter::addGlobal(const Type &value,const char *name)
{
	LuaBox::TypeIO<Type>::write(L, value);
	lua_setglobal(L, name);
}

#ifdef LUABOX_BINDER
template<> struct TypeIO<_Scripter::Object*>
{
	static inline bool write(Stream *L,_Scripter::Object * value)
	{ 
		value->toStack();
		return 1;
	}
	static inline _Scripter::Object* get(Stream *l,int i)
	{	
		return NULL;
	}
};

template<> struct TypeIO<LuaObject*>
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
	virtual ~TypeIO(){}
};
#endif	//LUABOX_BINDER

}	// namespace LuaBox
#endif
