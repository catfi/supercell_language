function f():void
{
    var i:uint32 = 0;
    while(i < 13)
    {
        i = i + 1;
        break;
        continue;
    }

    var j:uint32 = 0;
    do
    {
        j = j + 1;
        break;
        continue;
    } while(j < 17);
}
