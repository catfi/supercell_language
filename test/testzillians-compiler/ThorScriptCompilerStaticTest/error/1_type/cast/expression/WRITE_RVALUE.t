function g():int32
{
    return 13;
}

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="WRITE_RVALUE" } }
    g()=17;

    @static_test { expect_message={ level="LEVEL_ERROR", id="WRITE_RVALUE" } }
    19 = 23;
}
