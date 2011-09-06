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
	//var y:uint64 = 20;
	//var z:uint64 = x+y;
	
	/*
	if(z == 30)
	{
		return;
		z = 40;
	}
	elif(z == 40)
		z = 50;
	else
		z = 60;
	z = 70;
	*/
	/*
	while(x > 0)
	{
	   --x;
	}
	*/
	x = (x > 0) ? x+1 : x+2;
	
	return;
	//compiler_test.f1(x,y);
	//zillians.compiler_test.f1(x,y);
	//com.zillians.compiler_test.f1(x,y);
}
