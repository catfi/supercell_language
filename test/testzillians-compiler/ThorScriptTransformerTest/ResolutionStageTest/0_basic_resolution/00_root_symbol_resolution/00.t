function f0(x:int64, y:int64):int64
{
	return x + y;
}

function f1(x:int64, y:int64):int64
{
	return f0(x,y) + f0(x,y);
}

function f2(x:int64, y:int64):int64
{
	return f1(x,y) + f1(x,y);
}


function f3(x:int64, y:int64):int64
{
	return f4(x,y); // verify resolution to function declared after current line
}

function f4(x:int64, y:int64):int64
{
	if(x == 0 || y == 0)
		return 0;
	else 
		return f4(x-1, y-1); // verify resolution to local function (self)
}
