@static_test { expect_message={ level="LEVEL_WARNING", id="CONTROL_REACHES_END" } }
function f():int32
{
    var x:int32 = 17;

    {
        if(x)
        {
            { return 13; }
        }
        else
        {
        }
    }
}

function f2():int32
{
    var x:int32 = 17;

    {
        if(x)
        {
            { return 13; }
        }
        else
        {
            return 19;
        }
    }
}

function f3():int32
{
    var x:int32 = 17;

    {
        if(x)
        {
            { return 13; }
        }
        else
        {
        }
        return 19;
    }
}

@static_test { expect_message={ level="LEVEL_WARNING", id="CONTROL_REACHES_END" } }
function g():int32
{
    var x:int32 = 17;

    {
        switch(x)
        {
        case 13: { return 19; }
        case 17:
        }
    }
}

function g2():int32
{
    var x:int32 = 17;

    {
        switch(x)
        {
        case 13: { return 19; }
        case 17: return 23;
        }
    }
}

function g3():int32
{
    var x:int32 = 17;

    {
        switch(x)
        {
        case 13: { return 19; }
        case 17:
        }
        return 23;
    }
}
