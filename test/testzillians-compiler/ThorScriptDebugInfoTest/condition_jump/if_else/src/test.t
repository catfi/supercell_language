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

function test2(): void
{
	var x : int32 = 10;


	if (x > 10)
	{
		x = x + 3;
	}
	else
	{
		x = 4;
	}
}

function test3() : void
{
	var x : int32 = 10;
	var y : int32 = 10;

	if (x > 3)
	{
		x = 5;
		if ( y < 100)
		{
			y = x + y;
		}
	}

	var z : int32 = x + y;
}
