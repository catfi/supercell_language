function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="UNKNOWN_NAME", parameters={ id="x" } } }
    x=13;
}
