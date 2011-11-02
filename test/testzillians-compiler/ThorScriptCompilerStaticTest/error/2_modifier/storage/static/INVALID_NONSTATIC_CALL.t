class QWE
{
    // experimental group
    function g():void {}
    var g2:function():void; // TODO #1: can't init with g -- segfault

    // control group
    static function h():void {}
    @static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_STATIC_INIT" } }
    static var h2:function():void; // TODO #2: can't init with h -- emits UNINIT_REF

    // from static function..
    static function f():void
    {
        // experimental group
        // call non-static member function/variable
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ var_id="g" } } }
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func_id="g" } } }
        g();
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ var_id="g2" } } }
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func_id="g2" } } }
        @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="g2" } } } // TODO: remove after address TODO #1
        g2();

        // control group
        // call static member function/variable
        h();
        @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="h2" } } } // TODO: remove after address TODO #2
        h2();

        // call non-static local variable
        var p:function():void;
        @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="p" } } }
        p();

        // call static local variable
        @static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_STATIC_INIT" } }
        static var q:function():void;
        @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="q" } } }
        q();
    }
}

function r():void {}
var r2:function():void;

// from global function..
function f2():void
{
    // call global function/variable
    r();
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="r2" } } }
    r2();

    // call non-static local variable
    var s:function():void;
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="s" } } }
    s();

    // call static local variable
    @static_test { expect_message={ level="LEVEL_ERROR", id="MISSING_STATIC_INIT" } }
    static var t:function():void;
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="t" } } }
    t();
}
