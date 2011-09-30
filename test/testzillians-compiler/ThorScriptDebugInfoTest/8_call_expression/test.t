function test_function():uint32 
{
	test_function2(1, 3);
}

function test_function2(a:uint64, b:uint64):uint64
{
	var x:uint64 = 32;
	var y:uint64 = 33;

	x = x + y;	// x = 65
	
	y = y * b;	// y = 99
	x = x + a;	// x = 66

	x = x + 0;
	y = y + 0;
}
