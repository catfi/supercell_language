class test_me
{
	static function f0(x:int64, y:int64):int64
	{
		return x + y;
	}
	
	static function f1(x:int64, y:int64):int64
	{
		return f0(x,y) + f0(x,y); // verify direct static member function resolution can work
	}
	
	static function f2(x:int64, y:int64):int64
	{
		return f1(x,y) + f1(x,y);
	}
	
	static function f3(x:int64, y:int64):int64
	{
		return test_me.f4(x,y); // verify we can invoke static function through class name can work 
	}
	
	static function f4(x:int64, y:int64):int64
	{
		var t:int64 = 0;
		if(x == 0 || y == 0)
			return t;
		else
			return f4(x-1, y-1); // verify resolution to local function (self)
	}
}