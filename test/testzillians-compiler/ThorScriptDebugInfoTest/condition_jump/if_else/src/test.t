function test1():int32 
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

	if 
	(x < y)
	{ a = y; }
	else
	{
	}

	if (x == y)
	{
	}
	elif(x < y)
	{
		y = 3;
	}
	else
	{
	}
}
