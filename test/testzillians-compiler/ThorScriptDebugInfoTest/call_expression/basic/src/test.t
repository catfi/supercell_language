function sum(a:int32, b:int32): int32 
{
	var x : int32 = 0;
	x = a + b;
	return x;
}

function test1(): void
{
	var x : int32 =	sum(100, 200);
	x = x;
}
