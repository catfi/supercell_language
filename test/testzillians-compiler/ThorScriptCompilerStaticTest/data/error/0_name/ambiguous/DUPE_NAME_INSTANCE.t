@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="a" } } }
function f(a:uint32, a:uint32):void
{
}

@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="T" } } }
function f2<T, T>():void
{
}

//var x:uint32;

//@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="x" } } }
//var x:uint32;
