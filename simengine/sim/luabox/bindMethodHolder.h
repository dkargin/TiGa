#ifndef LUABOX_BIND_METHOD_HOLDER
#define LUABOX_BIND_METHOD_HOLDER
namespace LuaBox
{
	template<class Type> inline TypeBindings<Type> * getLuaType(TypeBindings<Type> *newptr=NULL);
	template<class Type> inline bool setWrapMeta(lua_State *l);
	// creates callable metauserdata. Would be <userdata> on LUA stack top
	template<class Holder> static bool make_holder(Holder *holder,lua_State *vm)
	{
		LuaBox::newudata(vm,holder);
		lua_newtable(vm);	// would be meta	
		{
			lua_pushstring(vm,"__call");
			lua_pushcfunction(vm,Holder::holder_call);
			lua_settable(vm,-3);

			lua_pushstring(vm,"__gc");
			lua_pushcfunction(vm,Holder::holder_gc);
			lua_settable(vm,-3);

			lua_pushstring(vm,"__index");
			lua_pushcfunction(vm,Holder::holder_index);
			lua_settable(vm,-3);

			lua_pushstring(vm,"__newindex");
			lua_pushcfunction(vm,Holder::holder_newindex);
			lua_settable(vm,-3);
		}
		lua_setmetatable(vm,-2);			
		return true;
	}
	// creates method holder and inserts it to Wrapper table, in methods talbe
	template<class Holder> static bool make_holder_method(Holder *holder,lua_State *vm,const char *type,const char *name)
	{
		// get wrapper name - stored in global namespace
		lua_getglobal(vm,type);	
		if(lua_isnil(vm,-1))
		{
			luaL_error(vm,"make_holder(%s::%s): invalid wrapper",type,name);
			lua_pop(vm,1);
			return false;
		}
		// get methods table
		lua_pushstring(vm,methodsEntry);
		lua_gettable(vm,-2);
		if(lua_isnil(vm,-1))
		{
			luaL_error(vm,"make_holder(%s::%s): no methods entry",type,name);
			lua_pop(vm,2);
			return false;
		}
		// write holder to type.methodsEntry.name
		lua_pushstring(vm,name);
		make_holder(holder,vm);
		lua_settable(vm,-3);
		lua_pop(vm,2);
		return true;
	}
	// creates method holder and inserts it to Wrapper table, in methods entry
	template<class Holder> static bool make_holder_constructor(Holder *holder,lua_State *vm,const char *type,const char *name)
	{
		// get Wrapper table
		lua_getglobal(vm,type);	
		if(lua_isnil(vm,-1))
		{
			luaL_error(vm,"make_holder(%s::%s): invalid wrapper",type,name);
			lua_pop(vm,1);
			return false;
		}
		// write holder to type.name
		lua_pushstring(vm,name);
		make_holder(holder,vm);
		lua_settable(vm,-3);
		lua_pop(vm,1);	// put back Wrapper table
		return true;
	}
	// get LuaCall pointer from lua stack
	template<class Holder> static Holder * get_holder(lua_State *l,int i)
	{
		//luaL_checktype(l, i, LUA_TUSERDATA);
		if(lua_type(l,i)==LUA_TUSERDATA)
			return *(Holder**)lua_touserdata(l,i);
		else
		{
			return NULL;
		}
	}
	template<class Type> struct Constructors
	{
		struct Base
		{
			virtual bool construct(lua_State *l, Type *ptr, int start)=0;
			virtual int args()const=0;
			static int holder_call(lua_State *l) // stack={holder, table,  args}
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __call\n");
				stackDump(l);
		#endif
				int args=lua_gettop(l)-2;		// arguments in stack			
				//Type * base=getLuaType<Type*>()->get(l,2);	// base pointer		
				Base *holder=get_holder<Base>(l,1);// function holder pointer
				if(args>holder->args())
					luaL_error(l,"LuaWrap holder: got %d args instead of %d",args,holder->args());			
				Type *result=(Type*)lua_newuserdata(l,sizeof(Type));
				//stackDump(l);
				holder->construct(l,result,3);
				//LuaBox::newudata(l,result);
				setWrapMeta<Type>(l);
				//getLuaType<Type*>()->write(l,result);
				return 1;
			}
			// holder meta __index - does nothing
			static int holder_index(lua_State *l)
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __index\n");
				stackDump(l);
		#endif
				return 0;
			}
			// holder meta __newindex - does nothing
			static int holder_newindex(lua_State *l)
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __newindex\n");
				stackDump(l);
		#endif
				return 0;
			}
			// holder meta __gc - removes LuaCall object
			static int holder_gc(lua_State *l)
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __gc\n");
				stackDump(l);
		#endif
				delete get_holder<Base>(l,-1);
				return 0;
			}	
		};
		template<int _args> struct Helper: public Base
		{
			int args()const
			{
				return _args;
			}
		};
		struct Impl_0: public Helper<0>
		{
			bool construct(lua_State *l, Type *ptr, int start)			
			{
				new(ptr) Type();
				return true;
			}
		};
		template<class arg0> struct Impl_1: public Helper<1>
		{
			bool construct(lua_State *l, Type *ptr, int start)			
			{
				new(ptr) Type(getLuaType<arg0>()->get(l,start));
				return true;
			}
		};
		template<class arg0,class arg1>	struct Impl_2: public Helper<2>
		{
			bool construct(lua_State *l, Type *ptr, int start)			
			{
				new(ptr) Type(getLuaType<arg0>()->get(l,start),getLuaType<arg1>()->get(l,start+1));
				return true;
			}
		};
		template<class arg0,class arg1,class arg2> struct Impl_3: public Helper<3>
		{
			bool construct(lua_State *l, Type *ptr, int start)			
			{
				new(ptr) Type(getLuaType<arg0>()->get(l,start),getLuaType<arg1>()->get(l,start+1),getLuaType<arg2>()->get(l,start+2));
				return true;
			}
		};
		template<class arg0,class arg1,class arg2,class arg3> struct Impl_4: public Helper<4>
		{
			bool construct(lua_State *l, Type *ptr, int start)			
			{
				new(ptr) Type(	getLuaType<arg0>()->get(l,start),
								getLuaType<arg1>()->get(l,start+1),
								getLuaType<arg2>()->get(l,start+2),
								getLuaType<arg3>()->get(l,start+3));
				return true;
			}
		};
		template<class arg0,class arg1,class arg2,class arg3,class arg4> struct Impl_5: public Helper<5>
		{
			bool construct(lua_State *l, Type *ptr, int start)			
			{
				new(ptr) Type(	getLuaType<arg0>()->get(l,start),
								getLuaType<arg1>()->get(l,start+1),
								getLuaType<arg2>()->get(l,start+2),
								getLuaType<arg3>()->get(l,start+3),
								getLuaType<arg4>()->get(l,start+4));
				return true;
			}
		};
	};
	template<class Type> struct Methods
	{		
		struct Base
		{
			virtual void call(lua_State *state,Type *object)=0;
			virtual int args()const=0;		// method arguments count
			virtual int res()const=0;		// method results count
			virtual ~Base(){}
			// holder meta __call
			static int holder_call(lua_State *l) // stack={holder, table,  args}
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __call\n");
				stackDump(l);
		#endif
				int args=lua_gettop(l)-2;		// arguments in stack			
				Type * base=getLuaType<Type*>()->get(l,2);	// base pointer		
				Base *holder=get_holder<Base>(l,1);// function holder pointer
				if(args>holder->args())
					luaL_error(l,"LuaWrap holder: got %d args instead of %d",args,holder->args());
				holder->call(l,base);
				return holder->res();
			}
			// holder meta __index - does nothing
			static int holder_index(lua_State *l)
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __index\n");
				stackDump(l);
		#endif
				return 0;
			}
			// holder meta __newindex - does nothing
			static int holder_newindex(lua_State *l)
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __newindex\n");
				stackDump(l);
		#endif
				return 0;
			}
			// holder meta __gc - removes LuaCall object
			static int holder_gc(lua_State *l)
			{
		#ifdef PRINT_LUA_WRAP
				printf("holder __gc\n");
				stackDump(l);
		#endif
				delete get_holder<Base>(l,-1);
				return 0;
			}	
		};
		// static arguments/results counter
		template<int _args,int _res> struct Helper: public Base
		{
			virtual int args() const
			{
				return _args;
			}
			virtual int res() const
			{
				return _res;
			}
		};

		// function holder for 0 arguments
		template<class ret>	struct Impl_0: public Helper<0,1>
		{
			typedef ret (Type::*fn_type)();
			fn_type fn;
			Impl_0(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				ret res=(object->*fn)();
				getLuaType<ret>()->write(l,res);
			}
		};

		// function holder for 0 arguments and "void" return type
		template<> struct Impl_0<void>: public Helper<0,1>
		{
			typedef void (Type::*fn_type)();
			fn_type fn;
			Impl_0(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				(object->*fn)();
			}
		};

		// function holder for 1 arguments
		template<class ret,class arg0> struct Impl_1: public Helper<1,1>
		{
			typedef ret (Type::*fn_type)(arg0);
			fn_type fn;
			Impl_1(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				getLuaType<ret>()->write(l,(object->*fn)(getLuaType<arg0>()->get(l,-1)));
			}
		};

		// function holder for 2 arguments
		template<class ret,class arg0,class arg1> struct Impl_2: public Helper<2,1>
		{
			typedef ret (Type::*fn_type)(arg0,arg1);
			fn_type fn;
			Impl_2(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				getLuaType<ret>()->write(l,(object->*fn)(
					getLuaType<arg0>()->get(l,-2),
					getLuaType<arg1>()->get(l,-1)));
			}
		};

		// function holder for 3 arguments
		template<class ret,class arg0,class arg1, class arg2> struct Impl_3: public Helper<3,1>
		{
			typedef ret (Type::*fn_type)(arg0,arg1,arg2);
			fn_type fn;
			Impl_3(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				getLuaType<ret>()->write(l,(object->*fn)(
					getLuaType<arg0>()->get(l,-3),
					getLuaType<arg1>()->get(l,-2),
					getLuaType<arg2>()->get(l,-1)));
			}
		};

		// function holder for 4 arguments
		template<class ret,class arg0,class arg1, class arg2, class arg3> struct Impl_4: public Helper<4,1>
		{
			typedef ret (Type::*fn_type)(arg0,arg1,arg2,arg3);
			fn_type fn;
			Impl_4(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				getLuaType<ret>()->write(l,(object->*fn)(
					getLuaType<arg0>()->get(l,-4),
					getLuaType<arg1>()->get(l,-3),
					getLuaType<arg2>()->get(l,-2),
					getLuaType<arg3>()->get(l,-1)));
			}
		};

		// function holder for 5 arguments
		template<class ret,class arg0,class arg1, class arg2, class arg3, class arg4> struct Impl_5: public Helper<4,1>
		{
			typedef ret (Type::*fn_type)(arg0,arg1,arg2,arg3,arg4);
			fn_type fn;
			Impl_5(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				getLuaType<ret>()->write(l,(object->*fn)(
					getLuaType<arg0>()->get(l,-5),
					getLuaType<arg1>()->get(l,-4),
					getLuaType<arg2>()->get(l,-3),
					getLuaType<arg3>()->get(l,-2),
					getLuaType<arg4>()->get(l,-1)));
			}
		};
		
		// function holder for 1 arguments and "void" return type
		template<class arg0> struct Impl_1<void,arg0>: public Helper<1,1>
		{
			typedef void (Type::*fn_type)(arg0);
			fn_type fn;
			Impl_1(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				(object->*fn)(getLuaType<arg0>()->get(l,-1));			
			}
		};
		
		// function holder for 2 arguments and "void" return type
		template<class arg0,class arg1>	struct Impl_2<void,arg0,arg1>: public Helper<2,0>
		{
			typedef void (Type::*fn_type)(arg0,arg1);
			fn_type fn;
			Impl_2(fn_type f):fn(f){}
			virtual void call(lua_State *l,Type *object)
			{
				(object->*fn)(getLuaType<arg0>()->get(l,-2),getLuaType<arg1>()->get(l,-1));
			}
		};
	};
}

#endif