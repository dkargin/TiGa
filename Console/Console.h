#pragma once;
#include <tchar.h>
#include <process.h>
#include <conio.h>
#include <map>
#include <functional>
#include <errno.h>
#include <stdlib.h>

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