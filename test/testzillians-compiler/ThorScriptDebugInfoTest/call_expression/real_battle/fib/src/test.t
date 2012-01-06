
function fib_for_loop(index: int32): int32
{
	if (index <= 1) return index;

	var i : int32;
	var ans : int32 = 0;
	var pre1 : int32 = 1;
	var pre2 : int32 = 0;
	for (i = 2; i <= index; i++)
	{
		ans = pre1 + pre2;
		pre2 = pre1;
		pre1 = ans;
	}
	return ans;
}

function fib_recursive(index: int32) : int32
{
	if (index <= 1) return index;
	
	return fib_recursive(index-1) + fib_recursive(index-2);
}

/**
 * Test function entry points
 */
function test1() : void
{
	var x : int32 = fib_for_loop(8);
	var y : int32 = fib_recursive(8);
	var result : int32 = 0;

	if (x == y)
	{
		result = 1;
	}
	else
	{
		result = 0;
	}
}
