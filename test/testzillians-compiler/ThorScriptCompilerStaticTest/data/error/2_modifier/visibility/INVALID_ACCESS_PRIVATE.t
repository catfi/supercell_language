class QWE
{
    private var x:uint32;
}

function f():void
{
    var qwe:QWE;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PRIVATE", parameter={ ID="x" } } }
    qwe.x=13;
}
