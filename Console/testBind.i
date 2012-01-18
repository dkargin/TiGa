%module TestBind;
%{
#include "../frostTools/autoPtr.hpp"
#include "Console.h"
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
//%RefCounted(TestClass)

//%feature("smartpointeraccess") TestClass;
//%attributeval(TestClass, int, LuaValue, getValue, setValue);

class TestClass
{
public:
	void doSomething();
};

%extend TestClass
{
	int value;
}

%{
int TestClass_value_get(TestClass * v) 
{
	return v->getValue();
}
void TestClass_value_set(TestClass * v, int val)
{
	v->setValue(val);
}
%}

class Derived : public TestClass
{
public:
};



template<class Target> struct SharedPtr
{
public:
	SharedPtr();
	~SharedPtr();
	SharedPtr(const SharedPtr & ptr );
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


%template(TestPtr) SharedPtr<TestClass>;
%{
	typedef SharedPtr<TestClass> TestPtr;
%}

%template(Test2Ptr) SharedPtr<Derived>;
%{
	typedef SharedPtr<Derived> Test2Ptr;
%}

TestPtr CreateClass(int val);
Test2Ptr CreateDerived(int val);