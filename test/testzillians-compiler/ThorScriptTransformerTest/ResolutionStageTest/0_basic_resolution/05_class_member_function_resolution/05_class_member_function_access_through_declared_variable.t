package com.zillians.compiler_test;

class test_me
{
	static function f0(x:uint64, y:uint64):uint64
	{
		return x + y;
	}
}

function test()
{
	var c:test_me;// = new test_me();
	c.f0();
	//c.c;
	test_me.f0();
}