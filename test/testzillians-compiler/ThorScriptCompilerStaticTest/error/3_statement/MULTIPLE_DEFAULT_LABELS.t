function f():void
{
    var x:int32 = 13;

    @static_test { expect_message={ level="LEVEL_ERROR", id="MULTIPLE_DEFAULT_LABELS" } }
    switch(x)
    {
    default:
    default:
    }
}
