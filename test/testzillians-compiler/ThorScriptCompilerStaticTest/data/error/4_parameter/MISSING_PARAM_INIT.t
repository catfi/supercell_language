@static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_PARAM_INIT", parameter={ PARAM_INDEX=1, FUNC="f" } } }
function f(a:uint32 = 13, b:uint32):void
{
}
