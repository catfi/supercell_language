function f():void
{
    if(true)
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_BREAK_TARGET" } }
        break;
    }
}
