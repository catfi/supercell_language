function f0(x:int64, y:int64):int64
{
	return x + y;
}
/*
function f1(x:int64, y:int64):int64
{
	return impl.f0(x,y) + impl.f0(x,y);
}

function f2(x:int64, y:int64):int64
{
	return impl.f1(x,y) + impl.f1(x,y);
}

function f3(x:int64, y:int64):int64
{
	return impl.f4(x,y);
}

function f4(x:int64, y:int64):int64
{
	if(x == 0 || y == 0)
		return 0;
	else 
		return impl.f4(x-1, y-1);
}*/