function f():void
{
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="MEMBER_OF_NONCLASS", parameter={ MEMBER_ID="y", PRIMARY_ID="x", PRIMARY_TYPE="uint32" } } }
    x.y = 1;
}
