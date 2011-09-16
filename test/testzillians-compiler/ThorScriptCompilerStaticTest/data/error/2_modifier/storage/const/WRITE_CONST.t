function f():void
{
    const x:uint32 = 13;

    @static_test { expect_message={ level="LEVEL_ERROR", id="WRITE_CONST", parameter={ VAR="x" } } }
    x=17;
}
