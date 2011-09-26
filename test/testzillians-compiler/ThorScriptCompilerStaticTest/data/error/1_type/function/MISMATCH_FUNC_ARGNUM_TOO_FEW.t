function g(a:int32):void {}

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="MISMATCH_CALL_ARGNUM_TOO_FEW", parameters={ func="g" } } }
    g();
}
