// type dupe
class QWE {}
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="QWE" } } }
class QWE {}

// global var dupe
var x:int32;
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="x" } } }
var x:int32;

// function dupe
function f():void {}
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="f" } } }
function f():void {}

// arg dupe
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="a" } } }
function g(a:int32, a:int32):void {}

// template arg dupe
@static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="T" } } }
function g2<T, T>():void {}

// local var dupe
function h(a:int32):void
{
    @static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="a" } } }
    var a:int32;

    var b:int32;
    @static_test { expect_message={ level="LEVEL_ERROR", id="DUPE_NAME", parameters={ id="b" } } }
    var b:int32;
}
