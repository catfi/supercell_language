@tag { key = 13 }
function f():void
{
}

@tag { key = 17 }
class my_class
{
    @tag { key = 19 }
    var x:uint32;

    @tag { key = 23 }
    function f():void
    {
    }
}

@tag { key = "hello", key2 = { key3 = "world" } } // NOTE: key-value pair accepted as value
function g():void
{
}
