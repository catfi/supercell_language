function g():uint32
{
}

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="WRITE_RVALUE" } }
    g()=13;
}
