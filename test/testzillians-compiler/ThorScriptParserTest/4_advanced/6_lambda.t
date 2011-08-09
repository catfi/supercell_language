function f(var x:function(uint32, uint32):uint32):void
{
}

function g():void
{
    var h:function(uint32, uint32):uint32 =
        function(x:uint32, y:uint32)
        {
            return x + y;
        };
    h(13, 17);
    f(h);
    f(function(x:uint32, y:uint32) { return x + y; }); // NOTE: infix function treated as name
}
