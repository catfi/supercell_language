function g(a:int32):void {}

function f():void
{
    var x:int32;

    var y:int32;
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF" } }
    y = x;

    // NOTE: FIX-ME! -- doesn't raise warning -- initial value different from binary assignment ?
    //@static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF" } }
    //var z:int32 = x;

    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF" } }
    g(x);
}
