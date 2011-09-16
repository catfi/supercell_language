class QWE {}

function f():void
{
    var qwe:QWE;

    @static_test { expect_message={ level="LEVEL_ERROR", id="NO_SUCH_MEMBER", parameter={ PRIMARY_TYPE="QWE", MEMBER_ID="x" } } }
    qwe.x=13;
}
