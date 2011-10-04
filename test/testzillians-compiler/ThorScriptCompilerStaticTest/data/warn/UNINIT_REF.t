function g(a:int32):void {}

function f():void
{
    var x:int32;

    var y:int32;
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="x" } } }
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="y" } } }
    y = x;

    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="x" } } }
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="z" } } }
    var z:int32 = x;

    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="x" } } }
    g(x);
}
