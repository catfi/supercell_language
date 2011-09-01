package com.zillians.compiler_test;

function f0(x:int32, y:int32):int32
{
	return x + y;
}

function f1(x:int32, y:int32):int32
{
	return f0(x,y) + f0(x,y);
}

function main():void
{
	var x:int32 = 10;
	var y:int32 = 20;
	var z:int32 = f1(x,y);
	
	//compiler_test.f1(x,y);
	//zillians.compiler_test.f1(x,y);
	//com.zillians.compiler_test.f1(x,y);
}
