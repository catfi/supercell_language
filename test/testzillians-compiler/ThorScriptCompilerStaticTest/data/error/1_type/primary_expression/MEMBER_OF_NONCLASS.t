function f():void
{
    var x:uint32;

    @static_test { expect_message={ level="LEVEL_ERROR", id="MEMBER_OF_NONCLASS", parameters={ member_id="y", primary_id="x", primary_type="uint32" } } }
    x.y = 1;
}
