function test1() : void
{
	var i : int32 = 5;
	var x : int32 = 0;
	var zero : int32 = 0;

	while (i > zero)
	{
		x++;
		if (x != zero) 
		{
			i--;
		}
	}
}
