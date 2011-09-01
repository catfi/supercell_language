package com.zillians.compiler_test;

function f0(x:int32, y:int32):int32
{
	return x + y;
}

function f1(x:int32, y:int32):int32
{
	return f0(x,y) + f0(x,y);
}

function f2(x:int32, y:int32):int32
{
	return f1(x,y) + f1(x,y);
}

function g0(