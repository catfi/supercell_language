function test1() : void
{
	var i : int32 = 0;
	var x : int32 = 0;

	for (i = 0; i < 5; i++)
	{
		x++;
	}

	x = x;
}

function test2() : void
{	
	var i : int32 = 0;
	var j : int32 = 0;
	var x : int32 = 0;


	for (i = 0; 
		i < 2; 
		i++)
	{

		for (j = 0; 
			 j < 2; 
			 j++)
		{
			x++;
		}
	}

	x = x; // dummy operation to examine variable
}
