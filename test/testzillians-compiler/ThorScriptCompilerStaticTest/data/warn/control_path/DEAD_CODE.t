function f():void
{
    return;

    @static_test { expect_message={ level="LEVEL_WARNING", id="DEAD_CODE" } }
    var x:uint32=13;
}
