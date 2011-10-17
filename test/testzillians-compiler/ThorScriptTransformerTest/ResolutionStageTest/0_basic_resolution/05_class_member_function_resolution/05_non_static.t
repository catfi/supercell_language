package com.zillians.compiler_test;

class test_me
{
	function f0(x:int64, y:int64):int64
	{
		return x + y;
	}
	
	function f1(x:int64, y:int64):int64
	{
		return f0(x,y) + f0(x,y); // verify direct member function resolution can work
	}
	
	function f2(x:int64, y:int64):int64
	{
		return f1(x,y) + f1(x,y);
	}
	
	function f3(x:int64, y:int64):int64
	{
		return f4(x,y); // verify this pointer can work 
	}
	
	function f4(x:int64, y:int64):int64
	{
		if(x == 0 || y == 0)
			return 0;
		else 
			return f4(x-1, y-1); // verify resolution to local function (self)
	}
}