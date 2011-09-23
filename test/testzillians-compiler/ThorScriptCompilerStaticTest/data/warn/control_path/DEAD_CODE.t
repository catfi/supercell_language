function f():void
{
    if(true)
    {
        {
            return;
        }

        @static_test { expect_message={ level="LEVEL_WARNING", id="DEAD_CODE" } }
        var x:uint32=13;

        return;
    }

    var y:uint32=17;

    return;

    @static_test { expect_message={ level="LEVEL_WARNING", id="DEAD_CODE" } }
    var z:uint32=19;
}
