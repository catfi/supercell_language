function f():void
{
    var x:uint32 = 17;
    switch(x) // NOTE: "break" not supported
    {
    case 13: x = 19; x = 23;
    case 17: x = 23; x = 29;
    default: x = 31; x = 37;
    }
}
