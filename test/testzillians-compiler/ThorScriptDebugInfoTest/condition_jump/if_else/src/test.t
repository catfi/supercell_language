function test_function():int32 
{
	var x:int64 = 32;
	var y:int64 = 42;
	var a:int64;
	var b:int64;
	var c:int64;

	if (x > y)
	{
		a = y;
		a = a / 2;
	}
	else
	{
		// should enter here
		a = x;
		a = a - 2;
	}

	if (x > y + 100)
	{
		b = 1000;
		b = a - 100;
	}
	elif (x == y - 10)
	{
		// should enter here
		b = y - x;
		b = b * 2;
	}
	else
	{
		b = 100; 
		b = 3;
	}
	
	if (x < y)
	{
		// should enter here
		c = 3;
	}
	elif (x > y)
	{
		c = 333;
	}
	else
	{
		c = 0;
	}
}
