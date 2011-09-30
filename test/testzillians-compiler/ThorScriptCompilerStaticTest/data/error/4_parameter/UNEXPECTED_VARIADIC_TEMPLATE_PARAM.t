@static_test { expect_message={ level="LEVEL_ERROR", id="UNEXPECTED_VARIADIC_TEMPLATE_PARAM" } }
function f<..., T>():void
{
}
