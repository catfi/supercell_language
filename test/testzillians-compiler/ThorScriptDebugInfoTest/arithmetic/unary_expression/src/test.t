function test_function():uint32 
{
	var x:uint64 = 3;
	var y:uint64;
	var z:uint64;

	y = x++;	// y = 3, x = 4
	y = ++x;	// y = 5, x = 5
	
	z = x--;	// z = 5, x = 4
	z = --x;	// z = 3, x = 3

	var b:uint64;
	b = ~x;		// b = -4
}
