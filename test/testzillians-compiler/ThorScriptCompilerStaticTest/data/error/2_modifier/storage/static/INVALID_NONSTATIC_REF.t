class QWE
{
    var x:uint32;

    static function f():void
    {
        @static_test { expect_message={ level="LEVEL_ERROR", id="INVALID_NONSTATIC_REF", parameters={ var="x" } } }
        x=13;
    }
}
