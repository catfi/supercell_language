class QWE
{
    function g():void {}
    var g2:function():void; // TODO: initialize with g -- segfaults now

    static function f():void
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ var_id="g" } } }
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func_id="g" } } }
        g();

        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ var_id="g2" } } }
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func_id="g2" } } }
        @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="g2" } } } // TODO: remove once g2 is initialized
        g2();
    }
}
