function test_function():int32 
{
	var x:int64 = 32;
	var y:int64 = 42;
	var z:int64;

	x = y - x;		// x = 10
	x = y * x;		// x = 420
//	z = (x > y) ? (y) : (x);	// z = 42

	z = x;			// z = 420
}
