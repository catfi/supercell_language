package qwe.asd.zxc;

function f():void
{
    var x:uint32 = 0;

    foreach(var i:uint32 in 0 ... 13)
    {
        //x = x + i;
        //break;
        //continue;
    }

    var j:uint32 = 0;
    foreach(j in 0 ... 17)
    {
        x = x + j;
        break;
        continue;
    }
}
