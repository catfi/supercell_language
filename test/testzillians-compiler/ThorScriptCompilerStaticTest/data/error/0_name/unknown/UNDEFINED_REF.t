function g():void;

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="UNDEFINED_REF", parameter={ ID="g" } } }
    g();
}
