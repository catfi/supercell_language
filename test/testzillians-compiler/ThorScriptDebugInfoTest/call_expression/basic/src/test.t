function test_function():int32 
{
	test_function2(1, 3);
}

function test_function2(a:int64, b:int64):int64
{
	var x:int64 = 32;
	var y:int64 = 33;

	x = x + y;	// x = 65
	
	y = y * b;	// y = 99
	x = x + a;	// x = 66
}
