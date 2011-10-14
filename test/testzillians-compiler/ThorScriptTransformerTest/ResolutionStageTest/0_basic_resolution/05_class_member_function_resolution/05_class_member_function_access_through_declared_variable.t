package com.zillians.compiler_test;

class test_me
{
	static function f0(x:int64, y:int64):int64
	{
		return x + y;
	}
}

function test():void
{
	var c:test_me;// = new test_me();
	c.f0();
	test_me.f0();
}