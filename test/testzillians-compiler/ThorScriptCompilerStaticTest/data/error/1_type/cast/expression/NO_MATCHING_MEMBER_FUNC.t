class QWE
{
    public function g():void {}
}

function f():void
{
    var qwe:QWE;

    @static_test { expect_message={ level="LEVEL_ERROR", id="NO_MATCHING_MEMBER_FUNC", parameter={ FUNC="g" } } }
    qwe.g(13);
}
