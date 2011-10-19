function f():void
{
    //@static_test { expect_message={ level="LEVEL_ERROR", id="UNEXPECTED_RETURN_VALUE" } }
    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_CONV" } }
    return 13;
}
