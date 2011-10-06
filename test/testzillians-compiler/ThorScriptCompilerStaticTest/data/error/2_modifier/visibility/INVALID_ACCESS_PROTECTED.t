class QWE
{
    protected var x:int32;
}

class ASD extends QWE
{
    function f():void
    {
        x=13;
    }
}

function g(qwe:QWE):void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PROTECTED", parameters={ id="x" } } }
    qwe.x=17;
}
