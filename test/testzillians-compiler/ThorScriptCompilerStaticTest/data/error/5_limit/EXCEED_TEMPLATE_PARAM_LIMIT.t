@static_test { expect_message={ level="LEVEL_ERROR", id="EXCEED_TEMPLATE_PARAM_LIMIT" } }
function f<T, T2, T3, T4, T5, T6, T7, T8, T9>():void
{
}
