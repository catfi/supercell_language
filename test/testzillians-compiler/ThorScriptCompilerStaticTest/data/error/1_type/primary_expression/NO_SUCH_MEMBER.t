class QWE {}

function f():void
{
    var qwe:QWE;

    @static_test { expect_message={ level="LEVEL_ERROR", id="NO_SUCH_MEMBER", parameters={ primary_type="QWE", member_id="x" } } }
    qwe.x=13;
}
