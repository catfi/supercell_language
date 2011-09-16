class QWE {}

function g(a:QWE):void
{
}

function f():void
{
    var x:uint32 = 13;

    @static_test { expect_message={ level="LEVEL_ERROR", id="MISMATCH_CALL_ARGTYPE", parameter={ ARG_TYPE="uint32", PARAM_TYPE="QWE", ARG_INDEX=0, FUNC="g" } } }
    g(x);
}
