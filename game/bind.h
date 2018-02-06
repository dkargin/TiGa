
/* -------- TYPES TABLE (BEGIN) -------- */


struct swig_type_info; 

void SWIG_Lua_NewPointerObj(lua_State* L,void* ptr,swig_type_info *type, int own);
extern swig_type_info * swig_types[];

#define SWIGTYPE_p_BodyDesc swig_types[0]
namespace LuaBox
{
	template<> struct TypeIO<BodyDesc *>
	{
		typedef BodyDesc * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_BodyDesc, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Controller swig_types[1]
namespace LuaBox
{
	template<> struct TypeIO<Controller *>
	{
		typedef Controller * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Controller, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Core swig_types[2]
namespace LuaBox
{
	template<> struct TypeIO<Application *>
	{
		typedef Application * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Core, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_CursorInfo swig_types[3]
namespace LuaBox
{
	template<> struct TypeIO<CursorInfo *>
	{
		typedef CursorInfo * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_CursorInfo, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_DWORD swig_types[4]
#define SWIGTYPE_p_Device swig_types[5]
namespace LuaBox
{
	template<> struct TypeIO<Device *>
	{
		typedef Device * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Device, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_DeviceDef swig_types[6]
namespace LuaBox
{
	template<> struct TypeIO<DeviceDef *>
	{
		typedef DeviceDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_DeviceDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Draw swig_types[7]
namespace LuaBox
{
	template<> struct TypeIO<Draw *>
	{
		typedef Draw * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Draw, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FILE swig_types[8]
#define SWIGTYPE_p_ForceField swig_types[9]
namespace LuaBox
{
	template<> struct TypeIO<ForceField *>
	{
		typedef ForceField * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ForceField, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_ForceFieldDef swig_types[10]
namespace LuaBox
{
	template<> struct TypeIO<ForceFieldDef *>
	{
		typedef ForceFieldDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ForceFieldDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxAnimation swig_types[11]
namespace LuaBox
{
	template<> struct TypeIO<FxAnimation *>
	{
		typedef FxAnimation * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxAnimation, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxAnimation2 swig_types[12]
namespace LuaBox
{
	template<> struct TypeIO<FxAnimation2 *>
	{
		typedef FxAnimation2 * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxAnimation2, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxEffect swig_types[13]
namespace LuaBox
{
	template<> struct TypeIO<FxEffect *>
	{
		typedef FxEffect * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxEffect, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxHolder swig_types[14]
namespace LuaBox
{
	template<> struct TypeIO<FxHolder *>
	{
		typedef FxHolder * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxHolder, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxManager swig_types[15]
namespace LuaBox
{
	template<> struct TypeIO<FxManager *>
	{
		typedef FxManager * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxManager, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxParticles swig_types[16]
namespace LuaBox
{
	template<> struct TypeIO<FxParticles *>
	{
		typedef FxParticles * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxParticles, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxPointer swig_types[17]
#define SWIGTYPE_p_FxSound swig_types[18]
namespace LuaBox
{
	template<> struct TypeIO<FxSound *>
	{
		typedef FxSound * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxSound, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_FxSprite swig_types[19]
namespace LuaBox
{
	template<> struct TypeIO<FxSprite *>
	{
		typedef FxSprite * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_FxSprite, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_GameObject swig_types[20]
namespace LuaBox
{
	template<> struct TypeIO<GameObject *>
	{
		typedef GameObject * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_GameObject, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_GameObjectDef swig_types[21]
namespace LuaBox
{
	template<> struct TypeIO<GameObjectDef *>
	{
		typedef GameObjectDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_GameObjectDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_HCHANNEL swig_types[22]
#define SWIGTYPE_p_HEFFECT swig_types[23]
#define SWIGTYPE_p_HGE swig_types[24]
#define SWIGTYPE_p_HTARGET swig_types[25]
#define SWIGTYPE_p_HTEXTURE swig_types[26]
#define SWIGTYPE_p_IO__DataBuffer swig_types[27]
namespace LuaBox
{
	template<> struct TypeIO<IO::DataBuffer *>
	{
		typedef IO::DataBuffer * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_IO__DataBuffer, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_IO__Stream swig_types[28]
namespace LuaBox
{
	template<> struct TypeIO<IO::Stream *>
	{
		typedef IO::Stream * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_IO__Stream, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_IO__StreamIn swig_types[29]
namespace LuaBox
{
	template<> struct TypeIO<IO::StreamIn *>
	{
		typedef IO::StreamIn * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_IO__StreamIn, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_IO__StreamOut swig_types[30]
namespace LuaBox
{
	template<> struct TypeIO<IO::StreamOut *>
	{
		typedef IO::StreamOut * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_IO__StreamOut, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Ihandle swig_types[31]
#define SWIGTYPE_p_Inventory swig_types[32]
namespace LuaBox
{
	template<> struct TypeIO<Inventory *>
	{
		typedef Inventory * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Inventory, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_InvokerContainer swig_types[33]
namespace LuaBox
{
	template<> struct TypeIO<InvokerContainer *>
	{
		typedef InvokerContainer * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_InvokerContainer, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Item swig_types[34]
namespace LuaBox
{
	template<> struct TypeIO<Item *>
	{
		typedef Item * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Item, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_ItemDef swig_types[35]
namespace LuaBox
{
	template<> struct TypeIO<ItemDef *>
	{
		typedef ItemDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ItemDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Level swig_types[36]
namespace LuaBox
{
	template<> struct TypeIO<Level *>
	{
		typedef Level * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Level, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Log swig_types[37]
namespace LuaBox
{
	template<> struct TypeIO<Log *>
	{
		typedef Log * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Log, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_LogFILE swig_types[38]
namespace LuaBox
{
	template<> struct TypeIO<LogFILE *>
	{
		typedef LogFILE * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_LogFILE, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_LogTrace swig_types[39]
namespace LuaBox
{
	template<> struct TypeIO<LogTrace *>
	{
		typedef LogTrace * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_LogTrace, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_LuaObject swig_types[40]
namespace LuaBox
{
	template<> struct TypeIO<LuaObject *>
	{
		typedef LuaObject * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_LuaObject, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_MountDef swig_types[41]
namespace LuaBox
{
	template<> struct TypeIO<MountDef *>
	{
		typedef MountDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_MountDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Mover swig_types[42]
namespace LuaBox
{
	template<> struct TypeIO<Mover *>
	{
		typedef Mover * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Mover, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_MoverCharacterDef swig_types[43]
namespace LuaBox
{
	template<> struct TypeIO<MoverCharacterDef *>
	{
		typedef MoverCharacterDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_MoverCharacterDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_MoverDef swig_types[44]
namespace LuaBox
{
	template<> struct TypeIO<MoverDef *>
	{
		typedef MoverDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_MoverDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_MoverVehicle swig_types[45]
namespace LuaBox
{
	template<> struct TypeIO<MoverVehicle *>
	{
		typedef MoverVehicle * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_MoverVehicle, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_MoverVehicleDef swig_types[46]
namespace LuaBox
{
	template<> struct TypeIO<MoverVehicleDef *>
	{
		typedef MoverVehicleDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_MoverVehicleDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Mt3x3 swig_types[47]
#define SWIGTYPE_p_ObjID swig_types[48]
#define SWIGTYPE_p_ObjectIterator swig_types[49]
namespace LuaBox
{
	template<> struct TypeIO<ObjectIterator *>
	{
		typedef ObjectIterator * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ObjectIterator, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_ObjectManager swig_types[50]
namespace LuaBox
{
	template<> struct TypeIO<ObjectManager *>
	{
		typedef ObjectManager * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ObjectManager, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_PerceptionDef swig_types[51]
namespace LuaBox
{
	template<> struct TypeIO<PerceptionDef *>
	{
		typedef PerceptionDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_PerceptionDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_PolyDesc swig_types[52]
namespace LuaBox
{
	template<> struct TypeIO<PolyDesc *>
	{
		typedef PolyDesc * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_PolyDesc, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Pose swig_types[53]
namespace LuaBox
{
	template<> struct TypeIO<Pose *>
	{
		typedef Pose * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Pose, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Projectile swig_types[54]
namespace LuaBox
{
	template<> struct TypeIO<Projectile *>
	{
		typedef Projectile * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Projectile, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_ProjectileDef swig_types[55]
namespace LuaBox
{
	template<> struct TypeIO<ProjectileDef *>
	{
		typedef ProjectileDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ProjectileDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Referenced swig_types[56]
namespace LuaBox
{
	template<> struct TypeIO<Referenced *>
	{
		typedef Referenced * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Referenced, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_SharedPtrT_FxEffect_t swig_types[57]
namespace LuaBox
{
	template<> struct TypeIO<SharedPtr< FxEffect > *>
	{
		typedef SharedPtr< FxEffect > * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_SharedPtrT_FxEffect_t, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_SharedPtrT_FxSprite_t swig_types[58]
namespace LuaBox
{
	template<> struct TypeIO<SharedPtr< FxSprite > *>
	{
		typedef SharedPtr< FxSprite > * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_SharedPtrT_FxSprite_t, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_SharedPtrT_GameObject_t swig_types[59]
#define SWIGTYPE_p_Solid swig_types[60]
#define SWIGTYPE_p_TargetingSystemDef swig_types[61]
namespace LuaBox
{
	template<> struct TypeIO<TargetingSystemDef *>
	{
		typedef TargetingSystemDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_TargetingSystemDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_ThrusterControlDef swig_types[62]
namespace LuaBox
{
	template<> struct TypeIO<ThrusterControlDef *>
	{
		typedef ThrusterControlDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ThrusterControlDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_ThrusterDef swig_types[63]
namespace LuaBox
{
	template<> struct TypeIO<ThrusterDef *>
	{
		typedef ThrusterDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_ThrusterDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_Unit swig_types[64]
namespace LuaBox
{
	template<> struct TypeIO<Unit *>
	{
		typedef Unit * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Unit, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_UnitDef swig_types[65]
namespace LuaBox
{
	template<> struct TypeIO<UnitDef *>
	{
		typedef UnitDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_UnitDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_VisionManager__Vision swig_types[66]
#define SWIGTYPE_p_Weapon swig_types[67]
namespace LuaBox
{
	template<> struct TypeIO<Weapon *>
	{
		typedef Weapon * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_Weapon, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_WeaponDef swig_types[68]
namespace LuaBox
{
	template<> struct TypeIO<WeaponDef *>
	{
		typedef WeaponDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_WeaponDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_WeaponTurret swig_types[69]
namespace LuaBox
{
	template<> struct TypeIO<WeaponTurret *>
	{
		typedef WeaponTurret * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_WeaponTurret, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_WeaponTurretDef swig_types[70]
namespace LuaBox
{
	template<> struct TypeIO<WeaponTurretDef *>
	{
		typedef WeaponTurretDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_WeaponTurretDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_World swig_types[71]
namespace LuaBox
{
	template<> struct TypeIO<World *>
	{
		typedef World * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_World, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p__Scripter__Object swig_types[72]
#define SWIGTYPE_p_b2Body swig_types[73]
#define SWIGTYPE_p_b2BodyDef swig_types[74]
namespace LuaBox
{
	template<> struct TypeIO<b2BodyDef *>
	{
		typedef b2BodyDef * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_b2BodyDef, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_b2FixtureDef swig_types[75]
#define SWIGTYPE_p_b2Vec2 swig_types[76]
#define SWIGTYPE_p_bool swig_types[77]
#define SWIGTYPE_p_dir swig_types[78]
#define SWIGTYPE_p_fRect swig_types[79]
namespace LuaBox
{
	template<> struct TypeIO<fRect *>
	{
		typedef fRect * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_fRect, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_float swig_types[80]
#define SWIGTYPE_p_float32 swig_types[81]
#define SWIGTYPE_p_hgeColorRGB swig_types[82]
namespace LuaBox
{
	template<> struct TypeIO<hgeColorRGB *>
	{
		typedef hgeColorRGB * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_hgeColorRGB, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_hgeFont swig_types[83]
namespace LuaBox
{
	template<> struct TypeIO<hgeFont *>
	{
		typedef hgeFont * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_hgeFont, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_hgeParticleSystemInfo swig_types[84]
namespace LuaBox
{
	template<> struct TypeIO<hgeParticleSystemInfo *>
	{
		typedef hgeParticleSystemInfo * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_hgeParticleSystemInfo, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_hgeRect swig_types[85]
#define SWIGTYPE_p_hgeSprite swig_types[86]
namespace LuaBox
{
	template<> struct TypeIO<hgeSprite *>
	{
		typedef hgeSprite * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_hgeSprite, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_int swig_types[87]
#define SWIGTYPE_p_lua_State swig_types[88]
namespace LuaBox
{
	template<> struct TypeIO<lua_State *>
	{
		typedef lua_State * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_lua_State, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_mat swig_types[89]
#define SWIGTYPE_p_pos swig_types[90]
#define SWIGTYPE_p_pos3 swig_types[91]
#define SWIGTYPE_p_rot swig_types[92]
#define SWIGTYPE_p_std__shared_ptrT_IO__DataBuffer_t swig_types[93]
#define SWIGTYPE_p_std__string swig_types[94]
namespace LuaBox
{
	template<> struct TypeIO<std::string *>
	{
		typedef std::string * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_std__string, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_std__vectorT_vec2f_t swig_types[95]
#define SWIGTYPE_p_vec swig_types[96]
#define SWIGTYPE_p_vec2f swig_types[97]
namespace LuaBox
{
	template<> struct TypeIO<Pose::vec *>
	{
		typedef Pose::vec * value_type;
		static bool valid(Stream * l, int i)
		{
			int type = lua_type(l,i);
			return type==LUA_TUSERDATA || type==LUA_TLIGHTUSERDATA;
		}
		static int write(Stream *l, value_type value)
		{
				SWIG_Lua_NewPointerObj(l, value, SWIGTYPE_p_vec2f, 0);
				return true;
		}
	private:
		virtual value_type get(Stream *l, int i)=0;
		virtual ~TypeIO(){}
	};
}
#define SWIGTYPE_p_vec2i swig_types[98]
#define SWIGTYPE_p_vec3f swig_types[99]
#define SWIG_TypeQuery(name) SWIG_TypeQueryModule(&swig_module, &swig_module, name)
#define SWIG_MangledTypeQuery(name) SWIG_MangledTypeQueryModule(&swig_module, &swig_module, name)
#define SWIG_MangledTypeQuery(name) SWIG_MangledTypeQueryModule(&swig_module, &swig_module, name)

/* -------- TYPES TABLE (END) -------- */

