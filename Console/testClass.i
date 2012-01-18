%module TestBind;
%{
#include "../frostTools/autoPtr.hpp"
#include "Console.h"
%}

%nodefaultctor; // Disable constructor auto binding
%nodefaultdtor;	// Disable destructor auto binding 

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

class Derived : public TestClass {};

template<class Target> struct SharedPtr
{
public:
	SharedPtr();
	~SharedPtr();
	SharedPtr(const SharedPtr & ptr );
	Target * operator -> ();
};

%template(TestPtr) SharedPtr<TestClass>;
%template(Test2Ptr) SharedPtr<Derived>;

SharedPtr<TestClass> CreateClass(int val);
SharedPtr<Derived> CreateDerived(int val);