var x:uint32;

function f():void
{
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="AMBIGUOUS_REF", parameter={ ID="x" } } }
    x = 13;
}
