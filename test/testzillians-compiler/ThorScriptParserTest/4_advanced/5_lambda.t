function f(x:function(int32, int32):int32):void
{
}

function g():void
{
    var h:function(int32, int32):int32 =
        function(x:int32, y:int32)
        {
            return x + y;
        };
    h(13, 17);
    f(h);
    f(function(x:int32, y:int32) { return x + y; }); // NOTE: infix function treated as name
}
