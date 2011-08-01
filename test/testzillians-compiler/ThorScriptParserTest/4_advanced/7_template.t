class my_class<T>
{
    var x:T;
}

function f<T>():void
{
    var x:T;
}

function g():void
{
    var x:my_class<uint32>;
    f<uint32>();
}
