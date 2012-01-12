function test1() : void
{
	var i : int32 = 5;
	var x : int32 = 0;

	while (i > 0)
	{
		x++;
		if (x != 0) 
		{
			i--;
		}
	}
	x = x;
}
