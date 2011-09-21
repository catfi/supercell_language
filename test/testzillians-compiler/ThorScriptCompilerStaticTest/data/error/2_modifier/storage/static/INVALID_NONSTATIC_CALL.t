class QWE
{
    function g():void {}

    static function f():void
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_CALL", parameters={ func="g" } } }
        g();
    }
}
