class QWE {}

function f():void
{
    var qwe:QWE;
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_CONV", parameter={ RHS_TYPE="uint32", LHS_TYPE="QWE" } } }
    qwe = x;
}
