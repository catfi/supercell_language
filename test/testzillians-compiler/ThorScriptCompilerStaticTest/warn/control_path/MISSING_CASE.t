enum MY_ENUM { p; q; r; }

function f(a:MY_ENUM):void
{
    var x:int32;

    @static_test { expect_message={ level="LEVEL_WARNING", id="MISSING_CASE", parameters={ id="r" } } }
    switch(a)
    {
    case MY_ENUM.p: x=13;
    case MY_ENUM.q: x=17;
    }
}
