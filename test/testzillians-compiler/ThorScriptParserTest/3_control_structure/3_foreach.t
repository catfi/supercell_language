function f():void
{
    var x:int32 = 0;

    foreach(var i:int32 in 0 ... 13)
    {
        x = x + i;
        break;
        continue;
    }

    var j:int32 = 0;
    foreach(j in 0 ... 17)
    {
        x = x + j;
        break;
        continue;
    }
}
