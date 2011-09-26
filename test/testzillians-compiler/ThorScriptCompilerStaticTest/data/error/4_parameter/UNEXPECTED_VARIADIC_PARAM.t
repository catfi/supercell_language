@static_test { expect_message={ level="LEVEL_ERROR", id="UNEXPECTED_VARIADIC_PARAM" } }
function f(a:..., b:int32):void
{
}
