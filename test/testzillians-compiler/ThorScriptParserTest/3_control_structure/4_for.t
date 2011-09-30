function f():void
{
    var x:int32 = 0;

    for(var i:int32 = 0; i <= 13; i++)
    {
        x = x + i;
        break;
        continue;
    }

    var j:int32 = 0;
    for(j = 0; j <= 17; j++)
    {
        x = x + j;
        break;
        continue;
    }
}
