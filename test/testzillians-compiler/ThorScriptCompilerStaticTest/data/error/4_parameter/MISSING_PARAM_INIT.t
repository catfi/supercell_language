@static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_PARAM_INIT", parameters={ param_index=2, func="f" } } }
function f(a:uint32 = 13, b:uint32):void
{
}
