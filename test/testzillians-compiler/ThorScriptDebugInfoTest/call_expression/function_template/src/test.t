function eval<T>(x : T) : void 
{
	var y : T;
	y = x;
}

function test() : void
{
	var x : int32 = 4000;
	eval(x);
}
