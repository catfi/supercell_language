class QWE
{
    // experimental group
    var x:int32;

    // control group
    static var y:int32 = 0;

    // from static function..
    static function f():void
    {
        // experimental group
        // reference non-static member variable
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ var_id="x" } } }
        x=13;

        // control group
        // reference static member variable
        y=17;

        // ref non-static local variable
        var p:int32 = 0;
        p;

        // ref static local variable
        static var q:int32 = 0;
        q;
    }
}

var r:int32;

// from global function..
function f2():void
{
    // ref global variable
    @static_test { expect_message={ level="LEVEL_WARNING", id="UNINIT_REF", parameters={ var_id="r" } } } // NOTE: bug -- r never init
    r;

    // ref non-static local variable
    var s:int32 = 0;
    s;

    // ref static local variable
    static var t:int32 = 0;
    t;
}
