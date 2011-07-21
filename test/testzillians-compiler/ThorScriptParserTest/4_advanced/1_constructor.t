class my_class
{
    public function new() // NOTE: no return type necessary
    {
    }

    public function new(var a:uint32, var b:float64) // NOTE: no return type necessary
    {
    }
}

function f():void
{
    var a:my_class = new my_class();
    var b:my_class = new my_class(13, 3.14159);
}
