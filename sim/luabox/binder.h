#pragma once

#define BaseNamespace LuaBox

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace LuaBox
{
	const char fieldsEntry[]="_fields";
	const char methodsEntry[]="_methods";
	const char wrapperEntry[]="_wrapper";
}

#include "../luabox/bindTypes.h"
#include "../luabox/bindMethodHolder.h"
#include "../luabox/bindFields.h"

#include <map>

typedef std::map<std::string,lua_CFunction> FnTable;

#ifdef PRINT_LUA_WRAP
#define BINDER_DEBUG(fmt, ...) {printf(fmt, ##__VA_ARGS__);printf("\n");}
#else
#define BINDER_DEBUG(fmt, ...)
#endif

namespace LuaBox
{
	/*class LuaStream: public DataStream
	{
		lua_State *l;
	public:
		LuaStream(lua_State *vm):l(vm){};
		virtual bool read(void *target,int size,const char *name)
		{
			assert(false && "no implementation");
			return false;
		}
		virtual void write(const void *target,int size,const char *name)
		{
			assert(false && "no implementation");
		}
		virtual void write(const int &value,const char *name)
		{
			lua_pushinteger(l,value);
		}
		virtual void write(const float &value,const char *name)
		{
			lua_pushnumber(l,value);
		}
		virtual bool read(int &value,const char *name)
		{
			value=lua_getnumber();
			return true;
		}
		virtual bool read(float &value,const char *name)
		{
			value=lua_getnumber(l,
		}
		virtual void beginObject(const char *type,const char *name)
		{
		}
	};*/
	template<class Type> inline void newudata(lua_State *l,Type *ptr) // creates new userdata to store object poiter
	{
		*(void **)lua_newuserdata(l, sizeof(void *)*2) = ptr;
	}

	template<class Type> inline void newludata(lua_State *l,Type *ptr)	// creates new lightuserdata to store object pointer
	{
#ifdef LuaBox_ONLYPTR
		*(void **)lua_newuserdata(l, sizeof(void *)) = ptr;
#else
		lua_pushlightuserdata(l,ptr);
#endif
	}

	template<class Type> inline Type * getudata(lua_State *l,int i)
	{
		return NULL;
	}

	// Storage for type-dependent wrapper name
	template<class Type> inline const char *wrapperName(const char *name=NULL)
	{
		static std::string local;
		if(name)
			local=name;
		return local.c_str();
	}

	template<class Type> inline bool setWrapMeta(lua_State *l)
	{
		const char *wrap=wrapperName<Type>();
		lua_getglobal(l,wrap);
		if(lua_isnil(l,-1))
		{			
			luaL_error(l,"Unable to find Wrap(%s)\n",wrap);
			return false;
		}
		lua_setmetatable(l, -2);
		lua_pushstring(l,"__metatable");
		lua_pushinteger(l,0);
		lua_settable(l,-3);
		return true;
	}
	// Base class for wrapper
	class LuaWrapBase
	{
	public:
		LuaWrapBase(lua_State *l,const char *name)
			:vm(l),baseName(name)
		{}	
	protected:
		// get object name
		static const char *getName(lua_State *l,int index)
		{
	#ifdef PRINT_LUA_WRAP
			stackDump(l);
	#endif
			int type=lua_type(l,index);
			luaL_checktype(l, index, LUA_TUSERDATA);
			lua_getmetatable(l,index);
			lua_pushstring(l,"name");
			lua_gettable(l,-2);
			const char *result=lua_tostring(l,-1);
			lua_pop(l,2);
			return result;
		}

		void writeMetaHeader(lua_State *l,const char *name)
		{
			lua_pushstring(l,"name");
			lua_pushstring(l,name);
			lua_settable(l,-3);
		}

		// add methods to table
		inline void storeFn(lua_State *l,const FnTable &table) const
		{
			for(FnTable::const_iterator it=table.begin();it!=table.end();it++)
			{
				lua_pushstring(l,it->first.c_str());
				lua_pushcfunction(l,it->second);
				lua_settable(l,-3);
			}
		}		
	protected:
		FnTable meta;
		lua_State *vm;
		std::string baseName;
	};	
	template<class Type>inline void push(lua_State *l,Type type)
	{
		LuaBox::getLuaType<Type>()->write(l,type);
	}
	template<class Key,class Type> inline void field(lua_State *l,Key key,Type type,int index=-1)
	{
		LuaBox::getLuaType<Key>()->write(l,key);
		LuaBox::getLuaType<Type>()->write(l,type);
		lua_settable(l,index);
	}
	inline void function(lua_State *l,const char *name,int (*fn)(lua_State *))
	{
		lua_pushcfunction(l,fn);
		lua_setglobal(l,name);
	}
}

template<class Base> class LuaWrap: public LuaBox::LuaWrapBase
{	
	struct LuaTypeRef: public LuaBox::TypeIO<Base&>
	{
		typedef typename TypeIO<Base&>::value_type value_type;
		value_type get(lua_State *l,int i)
		{				
			if(!valid(l,i))
				luaL_error(l,"LuaTypePtr::get() - invalid value\n");
#ifdef LuaBox_ONLYPTR
			return **( Base**)lua_touserdata(l,i);	
#else
			return *(Base*)lua_touserdata(l,i);
#endif			
		}	
		int write(lua_State *l,value_type base)
		{	
			LuaBox::newludata(l,reinterpret_cast<Base*>(&base));
			LuaBox::setWrapMeta<Base>(l);
			return 1;
		}
	}*typeRef;
	struct LuaTypeCRef: public LuaBox::TypeIO<const Base&>
	{
		typedef typename TypeIO<const Base&>::value_type value_type;
		value_type get(lua_State *l,int i)
		{				
			if(!valid(l,i))
				luaL_error(l,"LuaTypePtr::get() - invalid value\n");
#ifdef LuaBox_ONLYPTR
			return **( Base**)lua_touserdata(l,i);	
#else
			return *(Base*)lua_touserdata(l,i);
#endif			
		}	
		int write(lua_State *l,value_type base)
		{	
			LuaBox::newludata(l,(Base*)&base);
			LuaBox::setWrapMeta<Base>(l);
			return 1;
		}
	}*typeCRef;
	struct LuaTypePtr: public LuaBox::TypeIO<Base*>
	{
		value_type get(lua_State *l,int i)
		{	
			if(!valid(l,i))
				luaL_error(l,"LuaTypePtr::get() - invalid value\n");
#ifdef LuaBox_ONLYPTR
			return *( Base**)lua_touserdata(l,i);	
#else
			return (Base*)lua_touserdata(l,i);
#endif
		}	
		int write(lua_State *l,value_type base)
		{			
			//stackDump(l);
			LuaBox::newludata(l,base);
			LuaBox::setWrapMeta<Base>(l);
			return 1;
		}
	}*typePtr;
public:
	typedef LuaWrap<Base> my_type;
	typedef Field::Base<Base> FieldPtr;	
	
	static my_type & bind(lua_State *l,const char *n)
	{
		return *new my_type(l,n);
	}
	LuaWrap(lua_State *l,const char *n)
		:LuaWrapBase(l,n),typeRef(NULL),typePtr(NULL)
	{		
		LuaBox::wrapperName<Base>(n);
		//meta["__tostring"]=&__tostring;
		meta["__index"]=&__index;
		meta["__newindex"]=&__newindex;
		//meta["__call"]=&my_type::call;
		meta["__gc"]=&object__gc;
		//lua_newludata(l,this);
		lua_newtable(l);
		// write meta methods
		storeFn(l,meta);
		// write wrapper name
		lua_pushstring(l,"name");
		lua_pushstring(l,baseName.c_str());
		lua_settable(l,-3);			//table[name]=name
		// write object's methods list
		lua_pushstring(l,LuaBox::methodsEntry);
		lua_newtable(l);
		lua_settable(l,-3);
		// write object's fields list
		lua_pushstring(l,LuaBox::fieldsEntry);
		lua_newtable(l);
		lua_settable(l,-3);
		// write Wrapper poiter
		lua_pushstring(l,LuaBox::wrapperEntry);
		LuaBox::newudata(l,this);
		lua_newtable(l);
			lua_pushstring(l,"__gc");
			lua_pushcfunction(l,wrapper__gc);
			lua_settable(l,-3);
		lua_setmetatable(l,-2);	
		lua_settable(l,-3);
		// registering global
		lua_setglobal(l,baseName.c_str());	
		if(!LuaBox::getLuaType<Base&>())
			LuaBox::getLuaType<Base&>(typeRef=new LuaTypeRef());
		if(!LuaBox::getLuaType<const Base&>())
			LuaBox::getLuaType<const Base&>(typeCRef=new LuaTypeCRef());
		if(!LuaBox::getLuaType<Base*>())
			LuaBox::getLuaType<Base*>(typePtr=new LuaTypePtr());
	}
	~LuaWrap()
	{
		printf("LuaWrap::~LuaWrap(%s)\n",baseName.c_str());
		if(typeRef)delete typeRef;
		if(typePtr)delete typePtr;
	}
	typedef typename LuaBox::Methods<Base> Methods;
	typedef typename LuaBox::Constructors<Base> Constructors;

	template<class ret>	inline my_type& method(const char *name,ret (Base::*fn)(void))
	{		
		LuaBox::make_holder_method(new Methods::Impl_0<ret>(fn),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret>	inline my_type& method(const char *name,ret (Base::*fn)(void)const)
	{		
		typedef ret (Base::*trg)(void);
		LuaBox::make_holder_method(new Methods::Impl_0<ret>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret,class arg1> inline my_type& method(const char *name,ret (Base::*fn)(arg1))
	{		
		typedef ret (Base::*trg)(arg1);
		LuaBox::make_holder_method(new Methods::Impl_1<ret,arg1>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret,class arg1> inline my_type& method(const char *name,ret (Base::*fn)(arg1) const)
	{		
		typedef ret (Base::*trg)(arg1);
		LuaBox::make_holder_method(new Methods::Impl_1<ret,arg1>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret,class arg1,class arg2>inline my_type& method(const char *name,ret (Base::*fn)(arg1,arg2))
	{
		typedef ret (Base::*trg)(arg1,arg2);
		LuaBox::make_holder_method(new Methods::Impl_2<ret,arg1,arg2>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret,class arg1,class arg2, class arg3>inline my_type& method(const char *name,ret (Base::*fn)(arg1,arg2, arg3))
	{
		typedef ret (Base::*trg)(arg1,arg2,arg3);
		LuaBox::make_holder_method(new Methods::Impl_3<ret,arg1,arg2,arg3>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret,class arg1,class arg2, class arg3, class arg4>inline my_type& method(const char *name,ret (Base::*fn)(arg1,arg2, arg3, arg4))
	{
		typedef ret (Base::*trg)(arg1,arg2,arg3,arg4);
		LuaBox::make_holder_method(new Methods::Impl_4<ret,arg1,arg2,arg3,arg4>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}

	template<class ret,class arg1,class arg2, class arg3, class arg4, class arg5>inline my_type& method(const char *name,ret (Base::*fn)(arg1,arg2, arg3, arg4, arg5))
	{
		typedef ret (Base::*trg)(arg1,arg2,arg3,arg4,arg5);
		LuaBox::make_holder_method(new Methods::Impl_5<ret,arg1,arg2,arg3,arg4,arg5>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}
	/*
	template<class ret,class arg1,class arg2>inline my_type& method(const char *name,ret (Base::*fn)(arg1,arg2))
	{
		typedef ret (Base::*trg)(arg1,arg2);
		LuaBox::make_holder_method(new Methods::Impl_2<ret,arg1,arg2>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}*/
	template<class ret,class arg1,class arg2>inline my_type& method(const char *name,ret (Base::*fn)(arg1,arg2)const)
	{
		typedef ret (Base::*trg)(arg1,arg2);
		LuaBox::make_holder_method(new Methods::Impl_2<ret,arg1,arg2>(reinterpret_cast<trg>(fn)),vm,baseName.c_str(),name);
		return *this;
	}
	inline my_type& constructor(const char *name)
	{
		LuaBox::make_holder_constructor(new Constructors::Impl_0(),vm,baseName.c_str(),name);
		return *this;
	}
	template<class arg1> inline my_type& constructor(const char *name)
	{
		LuaBox::make_holder_constructor(new Constructors::Impl_1<arg1>(),vm,baseName.c_str(),name);
		return *this;
	}
	template<class arg1,class arg2> inline my_type& constructor(const char *name)
	{
		LuaBox::make_holder_constructor(new Constructors::Impl_2<arg1,arg2>(),vm,baseName.c_str(),name);
		return *this;
	}
	template<class arg1,class arg2,class arg3> inline my_type& constructor(const char *name)
	{
		LuaBox::make_holder_constructor(new Constructors::Impl_3<arg1,arg2,arg3>(),vm,baseName.c_str(),name);
		return *this;
	}
	template<class arg1,class arg2,class arg3,class arg4> inline my_type& constructor(const char *name)
	{
		LuaBox::make_holder_constructor(new Constructors::Impl_4<arg1,arg2,arg3,arg4>(),vm,baseName.c_str(),name);
		return *this;
	}
	template<class arg1,class arg2,class arg3,class arg4,class arg5> inline my_type& constructor(const char *name)
	{
		LuaBox::make_holder_constructor(new Constructors::Impl_5<arg1,arg2,arg3,arg4,arg5>(),vm,baseName.c_str(),name);
		return *this;
	}
	template<class Type> my_type& value(const char *name,Type Base::* offset,Type *_default=NULL)
	{
		addField(new Field::Simple<Base,Type>(offset,_default),name);
		return *this;
	}
	template<class Type> my_type& complex(const char *name,Type Base::* offset,Type *_default=NULL)
	{
		addField(new Field::Complex<Base,Type>(offset,_default),name);
		return *this;
	}
	// declare container type field
	template<class Type> my_type& container(const char *name,Type Base::* offset,Type *_default=NULL)
	{
		addField(new Field::Container<Target,Type>(offset,_default),name);
		return *this;
	}
	// declare get/set type field
	template<class Type> my_type& value(const char *name,	typename Field::GetSet<Base,Type>::FunctionGet get,
															typename Field::GetSet<Base,Type>::FunctionSet set,
															Type *_default=NULL)
	{
		addField(new Field::GetSet<Base,Type>(get,set,_default),name);
		return *this;
	}
protected:	
	virtual void addField(FieldPtr *pf,const char *name)
	{
		lua_getglobal(vm,baseName.c_str());
		if(lua_isnil(vm,-1))		
			luaL_error(vm,"Unable to find Wrap(%s)\n",baseName.c_str());
		else
		{
			lua_pushstring(vm,LuaBox::fieldsEntry);
			lua_gettable(vm,-2);
			lua_pushstring(vm,name);			
			LuaBox::newudata(vm,pf);
				lua_newtable(vm);	// would be meta					

				lua_pushstring(vm,"__gc");
				lua_pushcfunction(vm,field__gc);
				lua_settable(vm,-3);

				lua_setmetatable(vm,-2);
			lua_settable(vm,-3);
			lua_pop(vm,2);
		}
	}
	
	static int field__gc(lua_State *l)
	{
		BINDER_DEBUG("gc for field\n");
		//luaL_checktype(l, -1, LUA_TUSERDATA);
		delete *(FieldPtr**)lua_touserdata(l,-1);
		return 0;
	}
	static int wrapper__gc(lua_State *l)
	{
		BINDER_DEBUG("gc for wrapper\n");
		delete *(LuaWrap**)lua_touserdata(l,-1);
		return 0;
	}
	static int object__gc(lua_State *l)
	{
#ifdef LuaBox_ONLYPTR
		BINDER_DEBUG("gc for object %s", getName(l,-1));
		if(lua_objlen(l,-1)>sizeof(void*))
		{
			printf(" - delete object\n");
			delete *(Base**)lua_touserdata(l,-1);
		}
		else
			printf(" - leave object\n");
#else
		BINDER_DEBUG("gc for object %s\n", getName(l,-1));
		luaL_checktype(l, -1, LUA_TUSERDATA);
		Base *object=(Base*)lua_touserdata(l,-1);
		object->~Base();
		//delete (Base*)lua_touserdata(l,-1);
#endif
		//stackDump(L);
		return 0;
	}
protected:		
	// get object's field through Wrapper. 
	static bool getField(lua_State *l,const char *entry=NULL)	// called from __index
	{
		if(entry)
		{
			lua_pushstring(l,entry);// table,key,meta,"fields"
			lua_gettable(l,-2);		// table,key,meta,fields
			if(lua_isnil(l,-1))
			{
				luaL_error(l,"getField - no entry found\n");
			}
			lua_pushvalue(l,2);		// table,key,meta,fields,key
			//stackDump(l);
			lua_gettable(l,-2);		// table,key,meta,fields,fields[key]
			//stackDump(l);
			if(!lua_isnil(l,-1))	
				return true;
			lua_pop(l,2);
			return false;
		}
		else
		{
			lua_pushvalue(l,2);		// table,key,meta,fields,key
			lua_gettable(l,-2);		// table,key,meta,fields,fields[key]
			//stackDump(l);
			if(!lua_isnil(l,-1))	
				return true;
			lua_pop(l,1);
			return false;
		}		
	}	

	static FieldPtr * get_field_holder(lua_State *l,int i)	// called from in __index, __newindex 
	{
		luaL_checktype(l, i, LUA_TUSERDATA);
		return *(FieldPtr**)lua_touserdata(l,i);
	}

	// __index metamethod, sends back to base table
	static int __index(lua_State *l)	// table, key
	{
		BINDER_DEBUG("object__index(%s)",lua_tostring(l,-1));
		lua_getmetatable(l,1);		// table, key, Wrapper		
		//if(getField(l)								// Wrapper
		//	return 1;
		if(getField(l,LuaBox::fieldsEntry))	// try Wraper.fieldsEntry
		{
			Base * base=LuaBox::getLuaType<Base*>()->get(l,1);
			FieldPtr *ptr=get_field_holder(l,-1);
			if(ptr->canWrite())
			{
				ptr->write(l,base);
				return 1;
			}			
			return 0;
		}
		else 
			if(getField(l,LuaBox::methodsEntry))	// try Wrapper.methodsEntry
			return 1;
		else
			luaL_error(l,"invalid field");
		return 0;
	}
	// called from LUA when <userdata>.<key> = <value>
	static int __newindex(lua_State *l)
	{
		BINDER_DEBUG("adding new index");
		lua_getmetatable(l, 1);		// table, key, value ,meta
		lua_pushstring(l,LuaBox::fieldsEntry);
		lua_gettable(l,-2);			// table,key,value,meta,fields
		lua_pushvalue(l,2);			// table,key,meta,fields,key
		lua_gettable(l,-2);			// table,key,table.meta,table.meta.fields,table.meta.fields[key]
		if(lua_isnil(l,-1))
		{
			lua_pop(l,2);
			lua_pushvalue(l,-2);		// table, key, meta, key		
			lua_gettable(l,-2);			// table, key, meta, meta[key]
		}
		else
		{
			Base * base=LuaBox::getLuaType<Base*>()->get(l,1);
			FieldPtr *ptr=get_field_holder(l,-1);
			if(ptr->canRead())
			{
				lua_pushvalue(l,3);
				ptr->read(l,base);
			}
		}
		return 0;
	}
	/*
	static void logDebug(const char * msg)
	{
		#ifdef PRINT_LUA_WRAP
		printf(msg);
		printf("\n");
		#endif
	}*/
};


