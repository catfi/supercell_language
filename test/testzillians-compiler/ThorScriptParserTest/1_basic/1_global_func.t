function f():void
{
    var x:int32;
    x=1;
    f();
    return;
}

function g<x,y>(a:int32, b:float64=1):int32
{
    g(13, 3.14159);
    return 17;
    return g(19, 3.14159);
}
