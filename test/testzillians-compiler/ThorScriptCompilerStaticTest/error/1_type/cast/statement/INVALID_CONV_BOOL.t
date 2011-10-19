function g():void {}

function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_CONV_BOOL", parameters={ type="void" } } }
    if(g());
}
