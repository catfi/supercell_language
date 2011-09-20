@tag { key = 13, key2 = "aaa" }
function f():void
{
}

@tag { key = 17, key2 = "bbb" }
class my_class
{
    @tag { key = 19, key2 = "ccc" }
    var x:uint32;

    @tag { key = 23, key2 = "ddd" }
    function f():void
    {
    }
}

@tag { key = 29, key2 = "eee" }
@tag2 { key = 31, key2 = "fff" }
function g():void
{
}

@tag { key = 37, key2 = { key = 41, key2 = 43 } }
function h():void
{
}

@tag { key = 47, key2 = "ggg" }
interface iface
{
    function f():void;
}
