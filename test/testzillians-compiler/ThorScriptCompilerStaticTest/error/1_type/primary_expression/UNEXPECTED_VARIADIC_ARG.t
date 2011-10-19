function g<T, ...>(a:T, b:...):void
{
}

function f<T, ...>(a:T, b:...):void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="UNEXPECTED_VARIADIC_ARG" } }
    g(b, a);
}
