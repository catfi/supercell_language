function f():void
{
    var x:int32 = 17;
    if(x == 13)
    {
        x = 0;
    }
    elif(x == 17) // NOTE: not "else if"
    {
        x = 1;
    }
    else
    {
        x = 2;
    }
}
