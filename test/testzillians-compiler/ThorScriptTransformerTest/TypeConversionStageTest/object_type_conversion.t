package com.zillians.compiler_test;

class Base 
{ }

class Derived extends Base
{ }

function test():void {
	var base:Base;
	var derived:Derived;
	
	// okay because derived class is extended from base class (so derived class is one kind of base class)
	base = derived;
	
	// error! base class cannot be casted to derived class implicitly 
	derived = base;
}
