function g(a:int32):void {}

function f():void
{
    var x:int32;

    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_ARG" } }
    g(x);
}
