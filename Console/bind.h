
/* -------- TYPES TABLE (BEGIN) -------- */


struct swig_type_info; 

void SWIG_Lua_NewPointerObj(lua_State* L,void* ptr,swig_type_info *type, int own);
extern swig_type_info * swig_types[];

#define SWIGTYPE_p_Derived swig_types[0]
namespace LuaBox
{
	template<> struct TypeIO<Derived *>
	{
		typedef Derived * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Derived, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_SharedPtrT_Derived_t swig_types[1]
namespace LuaBox
{
	template<> struct TypeIO<SharedPtr< Derived > *>
	{
		typedef SharedPtr< Derived > * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_SharedPtrT_Derived_t, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_SharedPtrT_TestClass_t swig_types[2]
namespace LuaBox
{
	template<> struct TypeIO<SharedPtr< TestClass > *>
	{
		typedef SharedPtr< TestClass > * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_SharedPtrT_TestClass_t, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Test2Ptr swig_types[3]
#define SWIGTYPE_p_TestClass swig_types[4]
namespace LuaBox
{
	template<> struct TypeIO<TestClass *>
	{
		typedef TestClass * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_TestClass, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_TestPtr swig_types[5]
#define SWIG_TypeQuery(name) SWIG_TypeQueryModule(&swig_module, &swig_module, name)
#define SWIG_MangledTypeQuery(name) SWIG_MangledTypeQueryModule(&swig_module, &swig_module, name)
#define SWIG_MangledTypeQuery(name) SWIG_MangledTypeQueryModule(&swig_module, &swig_module, name)

/* -------- TYPES TABLE (END) -------- */

