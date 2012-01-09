function test1() : void
{
	var i : int32 = 0;
	var zero : int32 = 0;
	var max : int32 = 5;
	var x : int32 = 0;


	for (i = zero; i < max; i++)
	{
		x++;
	}
}

function test2() : void
{	
	var i : int32 = 0;
	var j : int32 = 0;
	var zero : int32 = 0;
	var max : int32 = 2;
	var x : int32 = 0;


	for (i = zero; 
		i < max; 
		i++)
	{

		for (j = zero; 
			 j < max; 
			 j++)
		{
			x++;
		}
	}

}
