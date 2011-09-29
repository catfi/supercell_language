function g(a:int32):void {}

function f():void
{
    var x:int32;

    // NOTE: FIX-ME! -- doesn't raise warning
    //@static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF" } }
    //var y:int32 = x;

    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF" } }
    g(x);
}
