package com.zillians.compiler_test;

class test_me
{
	function f0(x:uint64, y:uint64):uint64
	{
		return x + y;
	}
	
	function f1(x:uint64, y:uint64):uint64
	{
		return f0(x,y) + f0(x,y); // verify direct member function resolution can work
	}
	
	function f2(x:uint64, y:uint64):uint64
	{
		return f1(x,y) + f1(x,y);
	}
	
	function f3(x:uint64, y:uint64):uint64
	{
		return f4(x,y); // verify this pointer can work 
	}
	
	function f4(x:uint64, y:uint64):uint64
	{
		if(x == 0 || y == 0)
			return 0;
		else 
			return f4(x-1, y-1); // verify resolution to local function (self)
	}
}