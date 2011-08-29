package qwe.asd.zxc;

function f():void
{
    var x:uint32 = 17;
    switch(x) // NOTE: "break" not supported
    {
    case 13: x = 0;
    case 17: x = 1;
    default: x = 2;
    }
}
