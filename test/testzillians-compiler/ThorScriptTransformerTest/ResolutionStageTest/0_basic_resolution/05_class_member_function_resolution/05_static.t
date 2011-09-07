package com.zillians.compiler_test;

class test_me
{
	static function f0(x:uint64, y:uint64):uint64
	{
		return x + y;
	}
	
	static function f1(x:uint64, y:uint64):uint64
	{
		return f0(x,y) + f0(x,y); // verify direct static member function resolution can work
	}
	
	static function f2(x:uint64, y:uint64):uint64
	{
		return f1(x,y) + f1(x,y);
	}
	
	static function f3(x:uint64, y:uint64):uint64
	{
		return test_me.f4(x,y); // verify we can invoke static function through class name can work 
	}
	
	static function f4(x:uint64, y:uint64):uint64
	{
		if(x == 0 || y == 0)
			return 0;
		else
			return f4(x-1, y-1); // verify resolution to local function (self)
	}
}