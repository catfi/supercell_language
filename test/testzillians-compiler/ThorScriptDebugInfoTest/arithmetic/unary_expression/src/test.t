function test_function():int32 
{
	var x:int64 = 3;
	var y:int64;
	var z:int64;

	y = x++;	// y = 3, x = 4
	y = ++x;	// y = 5, x = 5
	
	z = x--;	// z = 5, x = 4
	z = --x;	// z = 3, x = 3

	var b:int64;
	b = ~x;		// b = -4
	b = b;
}
