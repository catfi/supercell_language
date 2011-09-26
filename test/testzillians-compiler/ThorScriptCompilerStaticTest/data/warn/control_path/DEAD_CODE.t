function f():void
{
    if(true)
    {
        {
            return;
        }

        @static_test { expect_message={ level="LEVEL_WARNING", id="DEAD_CODE" } }
        var x:int32=13;

        @static_test { expect_message={ level="LEVEL_WARNING", id="DEAD_CODE" } }
        return;
    }

    var y:int32=17;

    return;

    @static_test { expect_message={ level="LEVEL_WARNING", id="DEAD_CODE" } }
    var z:int32=19;
}
