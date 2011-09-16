function g(a:uint32):void {}

function f():void
{
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_ARG" } }
    g(x);
}
