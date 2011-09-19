package com.zillians.compiler_test.impl;

function f0(x:uint64, y:uint64):uint64
{
	return x + y;
}

function f1(x:uint64, y:uint64):uint64
{
	return impl.f0(x,y) + impl.f0(x,y);
}

function f2(x:uint64, y:uint64):uint64
{
	return impl.f1(x,y) + impl.f1(x,y);
}

function f3(x:uint64, y:uint64):uint64
{
	return impl.f4(x,y);
}

function f4(x:uint64, y:uint64):uint64
{
	if(x == 0 || y == 0)
		return 0;
	else 
		return impl.f4(x-1, y-1);
}