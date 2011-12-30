class my_class<T>
{
    var x:T;
}

function f<T>():void
{
    var x:T;
}

function f2<T, T2:f<T> >():void
{
}

function g():void
{
    var x:my_class<int32>;
    f<int32>();
    f2<int32>();
}

