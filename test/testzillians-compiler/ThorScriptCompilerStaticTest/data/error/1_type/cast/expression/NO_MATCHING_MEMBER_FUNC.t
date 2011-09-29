class QWE
{
    public function g():void {}
}

function f():void
{
    var qwe:QWE;

    @static_test { expect_message={ level="LEVEL_ERROR", id="NO_MATCHING_MEMBER_FUNC", parameters={ func_id="g" } } }
    qwe.g(13);
}
