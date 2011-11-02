function f():void
{
    @static_test { expect_message={ level="LEVEL_WARNING", id="INFINITE_RECURSION" } }
    f();
}
