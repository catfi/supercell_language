function f():void
{
    f();
    return;
}

function g(var x:uint32):uint32
{
    g(13);
    return 17;
    return g(19);
}
