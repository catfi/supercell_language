function f0(x:uint64, y:uint64):uint64
{
	return x + y;
}

function f1(x:uint64, y:uint64):uint64
{
	return f0(x,y) + f0(x,y);
}

function f2(x:uint64, y:uint64):uint64
{
	return f1(x,y) + f1(x,y);
}

function f3(x:uint64, y:uint64):uint64
{
	return f4(x,y); // verify resolution to function declared after current line
}

function f4(x:uint64, y:uint64):uint64
{
	if(x == 0 || y == 0)
		return 0;
	else 
		return f4(x-1, y-1); // verify resolution to local function (self)
}
