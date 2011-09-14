function f():void
{
    var x:uint32 = 0;

    for(var i:uint32 = 0; i <= 13; i++)
    {
        x = x + i;
        break;
        continue;
    }

    var j:uint32 = 0;
    for(j = 0; j <= 17; j++)
    {
        x = x + j;
        break;
        continue;
    }
}
