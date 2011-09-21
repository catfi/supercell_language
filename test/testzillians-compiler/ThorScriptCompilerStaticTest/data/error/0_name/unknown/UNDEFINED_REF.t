function g():void;

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="UNDEFINED_REF", parameters={ id="g" } } }
    g();
}
