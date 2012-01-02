class Base
{
	public function calculate(x:int32, y:int32) : int32
	{
		var z : int32;

		z = x + y;
		return z;
	}
}


class Sub extends Base
{
	public function calcualte2(x:int32, y:int32) : int32
	{
		var z : int32;

		z = x - y;
		return z;
	}
}

class TestCase
{
	public function test() : void
	{
		var f:Sub = new Sub();

		var value1:int32 = f.calculate(4, 3);		// value1 = 7
		var value2:int32 = f.calculate2(4, 3); 		// value = 1
	}
}
