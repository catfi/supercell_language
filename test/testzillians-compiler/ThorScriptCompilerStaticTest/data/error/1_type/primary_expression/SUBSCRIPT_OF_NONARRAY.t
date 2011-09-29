function f():void
{
    var x:int32;
    var y:int32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="SUBSCRIPT_OF_NONARRAY", parameters={ primary_type="int32", subscript_type="int32" } } }
    x[y] = 1;
}
