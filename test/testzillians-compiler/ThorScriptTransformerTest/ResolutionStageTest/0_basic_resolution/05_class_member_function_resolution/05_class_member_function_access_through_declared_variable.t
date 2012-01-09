class test_me
{
	static function f0(x:int64, y:int64):int64
	{
		return x + y;
	}
}

function test():void
{
	var c:test_me;// = new test_me();
    var v:int64;
	c.f0(v, v);
	test_me.f0(v, v);
}
