package com.zillians.compiler_test;

/*
function f0(x:int32, y:int32):int32
{
	return x + y;
}*/

/*
function f1(x:int32, y:int32):int32
{
	//return f0(x,y) + f0(x,y);
	return x + y;
}
*/

function main():void
{
	var x:uint64 = 10;
	var y:uint64 = 20;
	var z:uint64 = x+y;
	
	//compiler_test.f1(x,y);
	//zillians.compiler_test.f1(x,y);
	//com.zillians.compiler_test.f1(x,y);
}
