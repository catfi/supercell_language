function test1():int32 
{
	var x:int64 = 32;
	var y:int64 = 42;
	var a:int64 = 10;
	var b:int64 = 9;
	var c:int64 = 8;

	if (x > y)
	{
		a = y;
		a = a / 2;
	}
	else
	{
		// should enter here
		a = x;  // a = 32
		a = a - 2; // a = 30
	}

	if 
	(x < y)
	{ a = y; }	// a = 42
	else
	{
		b = 3; c = 2;
	}

	if (x == y)
	{
		b = 1;
		c = 0;
	}
	elif(x < y)
	{
		y = 3;		// y =3
	}
	else
	{
		x = a + b + c;
	}

	x = x;	// dummy operation to examine each variable
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

	x = x;	// dummy operation to examine each variable
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
	x = x;	// dummy operation to examine each variable
}
