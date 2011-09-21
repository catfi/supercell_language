class QWE {}

function f():void
{
    var qwe:QWE;
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_CONV", parameters={ rhs_type="uint32", lhs_type="QWE" } } }
    qwe=x;
}
