class my_class
{
}

function f():void
{
    var a:Object = new my_class(); // NOTE: default constructor
    var b:my_class = a as my_class;
}
