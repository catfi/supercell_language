function f():uint32
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_RETURN_VALUE", parameter={ TYPE="uint32" } } }
    return;
}