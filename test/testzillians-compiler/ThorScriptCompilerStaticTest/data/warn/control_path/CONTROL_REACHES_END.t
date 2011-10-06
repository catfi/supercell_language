@static_test { expect_message={ level="LEVEL_WARNING", id="CONTROL_REACHES_END" } }
function f():int32
{
    var x:int32 = 1;
    if(x)
    {
        return 13;
    }
    else
    {
    }
}
