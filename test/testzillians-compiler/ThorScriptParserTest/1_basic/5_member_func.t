class my_class
{
    function f():void
    {
        f();
        return;
    }

    function g():uint32
    {
        g(13);
        return 17;
        return g(19);
    }
}
