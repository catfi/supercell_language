function f(var x:function(uint32, uint32):uint32):void
{
}

function g():void
{
    var f2:function(uint32, uint32):uint32 = function(var x:uint32, var y:uint32) { return x + y; };
    f2(13, 17);
    f(f2);
    f(function(var x:uint32, var y:uint32) { return x + y; }); // NOTE: infix function treated as ref
}
