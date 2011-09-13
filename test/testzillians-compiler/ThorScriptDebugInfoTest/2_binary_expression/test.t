function test_function():uint32 
{
	var x:uint64 = 32;
	var y:uint64 = 42;
	var z:uint64;

	x = y - x;		// x = 10
	x = y * x;		// x = 420
	z = (x > y) ? (y) : (x);	// z = 42

	z = x;			// z = 420
}
