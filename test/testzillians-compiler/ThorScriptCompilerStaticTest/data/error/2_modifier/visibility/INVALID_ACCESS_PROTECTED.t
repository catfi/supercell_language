class QWE
{
    protected var x:int32;
}

class ASD extends QWE
{
    function f():void
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PROTECTED", parameters={ id="x" } } }
        x=13;
    }
}
