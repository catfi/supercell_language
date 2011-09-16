function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="UNKNOWN_NAME", parameter={ ID="x" } } }
    x=13;
}
