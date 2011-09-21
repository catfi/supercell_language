function f():void
{
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="CALL_NONFUNC", parameters={ func="x" } } }
    x();
}
