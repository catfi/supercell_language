class QWE {}

function f():void
{
    var qwe:QWE;
    var x:int32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_CONV", parameters={ rhs_type="int32", lhs_type="QWE" } } }
    qwe=x;
}
