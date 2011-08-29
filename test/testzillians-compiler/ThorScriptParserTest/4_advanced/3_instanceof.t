class my_class
{
}

function f():void
{
    var a:Object = new my_class(); // NOTE: default constructor
    var b:boolean = a instanceof my_class;
}
