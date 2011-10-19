function f():void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="UNKNOWN_NAME", parameters={ id="x" } } }
    x=13;
}

/* NOTE: is this an error ?
@static_test { expect_message={ level="LEVEL_ERROR", id="UNKNOWN_NAME", parameters={ id="..." } } }
function g(a:..., b:int32):void
{
}
*/
