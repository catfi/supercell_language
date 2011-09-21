function g():void {}

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="MISMATCH_CALL_ARGNUM", parameters={ func="g" } } }
    g(13);
}
