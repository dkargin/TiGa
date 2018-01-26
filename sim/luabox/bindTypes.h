#ifndef LUABOX_BIND_TYPES
#define LUABOX_BIND_TYPES

#include <typeinfo>

// Compiler-specific RTTI hacks
#ifdef _MSC_VER
#include <string.h>
inline const char * demangleName(const char * name)
{
    if (strncmp(name,"class",5)==0)
        return (name+6);
    else if(strncmp(name,"struct",6)==0)
        return (name+7);
    else
        return name;
}
#else
#include <ctype.h>
// G++ returns values in format <name len><actual name>, i.e typeid(Game).name() = "4Game";
inline const char * demangleName(const char * name)
{
    // skip numeric part
    while(isdigit(*name))
        name++;
    return name;
}
#endif

namespace LuaBox
{
	struct NullType
	{
		NullType(){}
	};

	typedef lua_State Stream;
    // This one is defined in swig
    bool pushObjectPtr(lua_State *l,void *object,const char *name);

    template<class Type> inline const char * typeName()
    {
        const std::type_info & info = typeid(Type);
        const char *name= info.name();
        return demangleName(name);
    }

    template<class Type> inline const char * typeName(Type *ptr)
    {
        return typeName<Type>();
    }

	template<class Type> struct TypeIO
	{
		typedef Type value_type;
		static bool valid(lua_State *l,int i)
		{
			int type=lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *luaVM,value_type value)
		{
			return pushObjectPtr(luaVM,value,typeName(value))?1:0;
		}
	private:
		virtual value_type get(Stream *l,int i)=0;
		virtual ~TypeIO(){}
	};

	template<class Type> inline TypeIO<Type> * getLuaType(TypeIO<Type> *newptr)
	{
		static TypeIO<Type> *storage=NULL;
		if(newptr!=NULL)
			storage=newptr;
		return storage;
	}
	///////////////////////////////////////////////////////////////
	// Proxies for standart types
	///////////////////////////////////////////////////////////////
	template<> struct TypeIO<int>
	{
		static inline int write(Stream *luaVM,const int &value)
		{
			lua_pushinteger(luaVM, value);
			return 1;
		}
		static inline int get(Stream *l,int i)
		{
			luaL_checktype(l, i, LUA_TNUMBER);
			return lua_tointeger(l,i);
		}
	};
	template<> struct TypeIO<float>
	{
		static inline int write(Stream *luaVM,const float &value)
		{
			lua_pushnumber(luaVM, value);
			return 1;
		}
		static inline float get(Stream *l,int i)
		{
			luaL_checktype(l, i, LUA_TNUMBER);
			return (float)lua_tonumber(l,i);
		}
	};
	template<> struct TypeIO<NullType>
	{
		static inline int write(Stream *luaVM,const NullType &value)
		{
			return 0;
		}
	private:
		static inline NullType get(Stream *l,int i);
	};
	template<> struct TypeIO<double>
	{
		static inline double get(Stream *l,int i)
		{		
			luaL_checktype(l, i, LUA_TNUMBER);
			return lua_tonumber(l,i);
		}
		static inline int write(Stream *luaVM,const double &value)
		{
			lua_pushnumber(luaVM, value);
			return 1;
		}
	};
	template<> struct TypeIO<bool>
	{
		static inline int write(Stream *luaVM,const bool &value)
		{
			lua_pushboolean(luaVM, value);
			return 1;
		}
		static inline bool get(Stream *l,int i)
		{	
			luaL_checktype(l,i,LUA_TBOOLEAN);
			return lua_toboolean(l,i)!=0;
		}
	};
	template<> struct TypeIO<std::string>
	{
		static inline int write(Stream *luaVM,const std::string &value)
		{
			lua_pushstring(luaVM, value.c_str());
			return 1;
		}
		static inline std::string get(Stream *l,int i)
		{	
			return lua_tostring(l,i);
		}
	};
	template<> struct TypeIO<const std::string &>
	{
		static inline int write(Stream *luaVM,const std::string &value)
		{
			lua_pushstring(luaVM, value.c_str());
			return 1;
		}
		static inline std::string get(Stream *l,int i)
		{	
			return lua_tostring(l,i);
		}
	};
	template<> struct TypeIO<const char*>
	{
		static inline int write(Stream *luaVM,const char *value)
		{
			lua_pushstring(luaVM, value);
			return 1;
		}
		static inline const char* get(Stream *l,int i)
		{	
			luaL_checktype(l,i,LUA_TSTRING);
			return lua_tostring(l,i);
		}
	};

	template<> struct TypeIO<void>
	{
		static inline int write(Stream *luaVM)
		{
			//lua_pushstring(luaVM, value);
			return 1;
		}
		
		static inline void get(Stream *l,int i)
		{	
			//luaL_checktype(l,i,LUA_TSTRING);
			//return lua_tostring(l,i);
		}
	};
}

#endif
