package com.zillians.compiler_test;

/*
function f0(x:int32, y:int32):int32
{
	return x + y;
}*/

function f1(x:int64, y:int64):int64
{
	--x;
	return (((x))) + ((y));
}

function main():void
{
	/*
	var x:int64 = 10;
	var y:int64 = 20;
	var z:int64 = f1(x, y);
	//var z:int64 = x + y;
	
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

	while(x > 0)
	{
	   --x;
	}
	x = (x > 0) ? x : x;
	*/
	
	var x:int64 = 10;
	x = (x > 0) ? x : x;
	return;
	//compiler_test.f1(x,y);
	//zillians.compiler_test.f1(x,y);
	//com.zillians.compiler_test.f1(x,y);
}

function entry():int64
{
	return f1(100,200);
}