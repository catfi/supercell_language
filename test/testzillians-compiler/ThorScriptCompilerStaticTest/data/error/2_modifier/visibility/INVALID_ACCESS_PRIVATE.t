class QWE
{
    private var x:int32;
}

class ASD extends QWE
{
    function f():void
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PRIVATE", parameters={ id="x" } } }
        x=13;
    }
}

function g(asd:ASD):void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PRIVATE", parameters={ id="x" } } }
    asd.x=17;
}
