class QWE {}

function g(a:QWE):void {}

function f():void
{
    var x:int32 = 13;

    @static_test { expect_message={ level="LEVEL_ERROR", id="MISMATCH_CALL_ARGTYPE", parameters={ arg_type="int32", param_type="QWE", arg_index=0, func="g" } } }
    g(x);
}
