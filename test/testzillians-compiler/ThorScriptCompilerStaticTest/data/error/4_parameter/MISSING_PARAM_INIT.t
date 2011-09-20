@static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_PARAM_INIT", parameters={ PARAM_INDEX=2, FUNC="f" } } }
function f(a:uint32 = 13, b:uint32):void
{
}
