function f():void
{
    var x:uint32;
    var y:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="SUBSCRIPT_OF_NONARRAY", parameter={ PRIMARY_TYPE="uint32", SUBSCRIPT_TYPE="uint32" } } }
    x[y] = 1;
}
