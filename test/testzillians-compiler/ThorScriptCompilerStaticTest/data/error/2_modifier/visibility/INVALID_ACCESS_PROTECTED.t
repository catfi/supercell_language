class QWE
{
    protected var x:int32;
}

class ASD extends QWE {}

function f():void
{
    var asd:ASD;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PROTECTED", parameters={ id="x" } } }
    asd.x=13;
}
