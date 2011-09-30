class QWE
{
    function g():void {}
    var g2:function():void;

    static function f():void
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func_id="g" } } }
        g();

        // NOTE: FIX-ME! -- LOG4CXX does not support multiple errors on 1 line
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func_id="g2" } } }
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ func_id="g2" } } }
        @static_test { expect_message={ level="LEVEL_ERROR", id="UNINIT_REF", parameters={ func_id="g2" } } }
        g2();
    }
}
