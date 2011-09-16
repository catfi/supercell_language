class QWE
{
    protected var x:uint32;
}

class ASD extends QWE {}

function f():void
{
    var asd:ASD;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PROTECTED", parameter={ ID="x" } } }
    asd.x=13;
}
