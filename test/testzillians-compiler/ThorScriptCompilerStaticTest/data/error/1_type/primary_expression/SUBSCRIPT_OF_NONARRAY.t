function f():void
{
    var x:uint32;
    var y:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="SUBSCRIPT_OF_NONARRAY", parameters={ primary_type="uint32", subscript_type="uint32" } } }
    x[y] = 1;
}
