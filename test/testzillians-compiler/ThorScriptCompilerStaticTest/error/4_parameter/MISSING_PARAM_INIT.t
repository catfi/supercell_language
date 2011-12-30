@static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_PARAM_INIT", parameters={ param_index=2, func_id="f" } } }
function f(a:int32 = 13, b:int32):void
{
}
