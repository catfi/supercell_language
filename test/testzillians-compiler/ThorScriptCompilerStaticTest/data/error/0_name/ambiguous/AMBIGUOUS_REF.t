var x:uint32;

function f():void
{
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="AMBIGUOUS_REF", parameters={ id="x" } } }
    x=13;
}
