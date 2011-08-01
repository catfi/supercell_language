function f():void
{
    f();
    return;
}

function g(a:uint32, b:float64):uint32
{
    g(13, 3.14159);
    return 17;
    return g(19, 3.14159);
}
