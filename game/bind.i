%module TiGa

%include "stl.i"

%{
#include "iup.h"
#include "iuplua.h"
%}

%nodefaultctor; // Disable constructor auto binding
%nodefaultdtor;	// Disable destructor auto binding 

%define %RefCounted(TYPE...)
%typemap(in,checkfn="lua_isuserdata") TYPE*, TYPE&
%{
	// obtain pointer from lua
	$1 = NULL;
	{
		Referenced::BaseObjectInfo<TYPE> * objectInfo = NULL;
		SWIG_ConvertPtr(L, $input, (void**)&objectInfo, $descriptor, $disown);
		$1 = objectInfo->data; //&argp, $descriptor, %convertptr_flags
	}
	//$1.assign(lua_tostring(L,$input),lua_strlen(L,$input));
%}

%typemap(out) TYPE*, TYPE&
%{ 
	// do something important to insert pointer to LUA
	//lua_pushlstring(L,$1.data(),$1.size()); SWIG_arg++;
	{
		Referenced::BaseObjectInfo<TYPE> * objectInfo = Referenced::getObjectInfo<TYPE>($1);
		SWIG_NewPointerObj(L, (void**)&objectInfo , $descriptor, $owner); SWIG_arg++;
	}
%}
%enddef

struct lua_State {};
// just a trick to call half-native luaC functions:
// int fn(lua_State *);
// or half-native class methods, removing first parameter (this) from stack
// void someClass::method(lua_State * );
%typemap(in) lua_State * 
%{ $1=L;lua_remove(L,1); %}

%typemap(in) const _Scripter::Object &, _Scripter::Object, _Scripter::Object &
%{ 
	// init lua reference from stack
	$1=new _Scripter::Object(_Scripter::Object::fromStack(L,$input)); 
%}

%typemap(check) const _Scripter::Object &, _Scripter::Object, _Scripter::Object &
%{
	// do no checks for _Scripter::Object
%}

%typemap(freearg) const _Scripter::Object &, _Scripter::Object, _Scripter::Object &
%{
	delete $1;
%}

%typemap(in,checkfn="lua_isuserdata") void *
%{
	$1 = lua_touserdata(L,$input);
%}

%typemap(out) void *
%{ lua_pushlightuserdata(L,$1); SWIG_arg++; %}

%typemap(in,checkfn="iuplua_checkihandle") Ihandle *
%{
	$1 = iuplua_checkihandle(L,$input);
%}


%typemap(out) Ihandle *
%{ iuplua_pushihandle(L,$1); SWIG_arg++; %}

class Log
{
public:
	virtual void line(int threat,const char *format);
	inline int levelInc();
	inline int levelDec();
	void setThreat(int threat);
};

class LogFILE: public Log
{
public:
	FILE *log;
	bool local;
	~LogFILE();
};

class LogTrace: public Log
{
public:
	LogTrace(){}
	~LogTrace();
};

Log * g_logger;

class Referenced {};

template<class Target> struct SharedPtr
{
public:
	SharedPtr();
	~SharedPtr();
	//operator Target * ();
	Target * operator -> ();
	%extend 	
	{
		bool operator==(const SharedPtr<Target> & ptr)const
		{
			return ptr.get() == $self->get();
		}
		bool operator!=(const SharedPtr<Target> & ptr)const
		{
			return ptr.get() == $self->get();
		}
	}
};
namespace std
{
	template<class Target> struct shared_ptr
	{
	public:
		shared_ptr();
		~shared_ptr();
		//operator Target * ();
		Target * operator -> ();
		%extend 	
		{
			bool operator==(const shared_ptr<Target> & ptr)const
			{
				return ptr.get() == $self->get();
			}
			bool operator!=(const shared_ptr<Target> & ptr)const
			{
				return ptr.get() == $self->get();
			}
		}
	};
	template<class Target> struct weak_ptr
	{
	public:
		weak_ptr();
		~weak_ptr();
		//operator Target * ();
		Target * operator -> ();
		%extend 	
		{
			bool operator==(const weak_ptr<Target> & ptr)const
			{
				return ptr == *this;
			}
			bool operator!=(const weak_ptr<Target> & ptr)const
			{
				return ptr != *this;
			}
		}
	};	
}
namespace IO
{
	class DataBuffer: public Referenced
	{
	public:
		DataBuffer();
		~DataBuffer();

		size_t size()const;

		const char * data() const;
		// calculate power of 2 size
		static size_t calcSize(size_t newsize);
		// resize data buffer
		void resize(size_t newsize = minSize);

		void grow(size_t amount);

		size_t write(size_t position, const void *data, size_t size);

		size_t read(void * data, size_t position, size_t size) const;
	};

	typedef std::shared_ptr<DataBuffer> BufferPtr;
	
	class Stream
	{
	public:
		bool eof();
		
		size_t position()const;

		void position(size_t p);

		void rewind();

		int size()const;
		// free bytes left
		size_t left()const;
	};

	class StreamIn : public IO::Stream
	{
	public:
		//StreamIn(BufferPtr buffer);
	};

	class StreamOut : public IO::Stream
	{
	public:
		//StreamOut(BufferPtr buffer);
	};

	bool readBool(StreamIn & stream);
	int readInt(StreamIn & stream);
	float readFloat(StreamIn & stream);
	std::string readString(StreamIn & stream);

	void writeBool(StreamOut & stream, bool value);
	void writeInt(StreamOut & stream, int value);
	void writeFloat(StreamOut & stream, float value);
	void writeString(StreamOut & stream, std::string value);
}

// store value in lua registry
%native(regStore) int regStore(lua_State *l);
// get value from lua registry
%native(regGet) int regGet(lua_State *l);
// clear registry entry
%native(regFree) int regFree(lua_State *l);
// debug console print
%native(debugTrace) int debugTrace(lua_State* L);
// binary OR for lua
%native(binOR) int binOR(lua_State * L);
// binary AND for lua
%native(binAND) int binAND(lua_State * L);
// binary AND for lua. returns NOT for each operand
%native(binNOT) int binNOT(lua_State * L);
// 
%native(convertHandle) int convertHandle(lua_State *l);
// convert absolute path to relative
%native(toAppRelative) int lua_toAppRelative(lua_State * l);
// get base name of SWIG-wrapped object
%native(SWIG_base_name) int SWIG_base_name(lua_State *L);

%{
// store value in lua registry
int regStore(lua_State *l)
{
	int ref=luaL_ref(l,LUA_REGISTRYINDEX);
	lua_pushinteger(l,ref);
	return 1;
}
// get value from lua registry
int regGet(lua_State *l)
{	
	luaL_checktype(l,-1,LUA_TNUMBER);
	int ref=lua_tointeger(l,-1);
	lua_rawgeti(l,LUA_REGISTRYINDEX,ref);
	return 1;
}
// clear registry entry
int regFree(lua_State *l)
{
	luaL_checktype(l,-1,LUA_TNUMBER);
	int ref=lua_tointeger(l,-1);
	luaL_unref(l,LUA_REGISTRYINDEX,ref);
	return 0;
}

extern Log * g_logger;
int debugTrace(lua_State* L)
{
	std::string Buf;
	int n=lua_gettop(L)+1;

	lua_getglobal(L,"tostring");
	for(int i=1;i<n;i++)
	{
		lua_pushvalue(L,-1);
		lua_pushvalue(L,i);
		lua_call(L,1,1);
		Buf+=lua_tostring(L,-1);
		lua_pop(L,1);
	}
	if(g_logger)g_logger->line(0,Buf.c_str());
	return 0;
}

// binary OR for lua
int binOR(lua_State * L)
{
	int top = lua_gettop(L);
	luaL_checktype(L,1,LUA_TNUMBER);
	int result = lua_tointeger(L,1);
	for (int i = 2; i <= top; i++) 
	{ 
		luaL_checktype(L,i,LUA_TNUMBER);
		result |= lua_tointeger(L,i);
	}
	lua_pushinteger(L,result);
	return 1;
}

// binary AND for lua
int binAND(lua_State * L)
{
	int top = lua_gettop(L);
	luaL_checktype(L,1,LUA_TNUMBER);
	int result = lua_tointeger(L,1);
	for (int i = 2; i <= top; i++) 
	{ 
		luaL_checktype(L,i,LUA_TNUMBER);
		result &= lua_tointeger(L,i);
	}
	lua_pushinteger(L,result);
	return 1;
}

// binary AND for lua. returns NOT for each operand
int binNOT(lua_State * L)
{
	int top = lua_gettop(L);	
	for (int i = 1; i <= top; i++) 
	{ 
		luaL_checktype(L,i,LUA_TNUMBER);
		int val = lua_tointeger(L,i);
		lua_pushinteger(L,~val);
	}	
	return top;
}

int convertHandle(lua_State *l)
{
	int top = lua_gettop(l);
	int type = lua_type(l,-1);
	void * val = lua_touserdata(l, -1);
	lua_pushinteger(l, (int)val);
	return 1;
}

int lua_toAppRelative(lua_State * l)
{
	luaL_checktype(l,-1,LUA_TSTRING);
	std::string res = toAppRelative(lua_tostring(l,-1));
	lua_pushstring(l,res.c_str());
	return 1;
}

int SWIG_base_name(lua_State *L)
{
	luaL_checktype(L,-1,LUA_TSTRING);
	const char * name = lua_tostring(L,-1);
	int len = strlen(name);
	const char * ptr = name + len;
	while(ptr-- != name && *ptr != '.');
	if(name != ptr)
		lua_pushstring(L,ptr+1);
	else
		lua_pushstring(L,ptr);
	return 1;
}

bool pushObjectPtr(lua_State *l,void *object,const char *name)
{
	static char mangled[255];
	strcpy(mangled,"_p_");
	strcpy(mangled+3,name);
	swig_type_info *type=SWIG_TypeQuery(mangled);
	if(!type)return false;
	SWIG_Lua_NewPointerObj(l,object,type,0);
	return true;	
}

%}
/// Core bindings
%include bindMath.i
%include bindHGE.i
%include bindFx.i
%include bindDevices.i
%include bindObjects.i

%include bindCore.i

/// Game-specific bindings
%include bindGame.i
