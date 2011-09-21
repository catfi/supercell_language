function g():void {}

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="MISMATCH_CALL_ARGNUM_TOO_MANY", parameters={ func="g" } } }
    g(13);
}
