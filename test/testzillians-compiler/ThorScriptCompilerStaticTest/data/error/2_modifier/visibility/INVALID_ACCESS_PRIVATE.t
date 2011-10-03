class QWE
{
    private var x:int32;
}

function f():void
{
    var qwe:QWE;// = new QWE;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_ACCESS_PRIVATE", parameters={ id="x" } } }
    qwe.x=13;
}
