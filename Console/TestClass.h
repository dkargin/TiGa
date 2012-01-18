#pragma once;

#include "../frostTools/autoPtr.hpp"

class TestClass : public Referenced
{
	int value;
public:
	TestClass(int value)
		:value(value)
	{
		printf("TestClass(%d)\n", value);
	}
	~TestClass()
	{
		printf("~TestClass(%d)\n", value);
	}
	TestClass(const TestClass & tc) : value(tc.value) 
	{
		printf("TestClass(%d) copy\n", value);
	}
	int getValue()
	{
		return value;
	}
	void setValue(int value)
	{
		this->value = value;
	}
	void doSomething()
	{
		printf("very important action %d\n", value);
	}
};

class Derived : public TestClass
{
public:
	Derived(int val) : TestClass(val) {}
};

inline SharedPtr<TestClass> CreateClass(int val)
{
	return new TestClass(val);
}

inline SharedPtr<Derived> CreateDerived(int val)
{
	return new Derived(val);
}